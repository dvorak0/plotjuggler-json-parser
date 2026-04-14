#include "parser_ros2_string_json.h"

#include <QDebug>

#include <cstdint>
#include <cstring>
#include <stdexcept>

using namespace PJ;

namespace
{
uint32_t ReadLe32(const uint8_t* ptr)
{
  return (uint32_t(ptr[0]) << 0) | (uint32_t(ptr[1]) << 8) | (uint32_t(ptr[2]) << 16) |
         (uint32_t(ptr[3]) << 24);
}
}

ROS2StringJsonMessageParser::ROS2StringJsonMessageParser(const std::string& topic_name,
                                                         PJ::PlotDataMapRef& data)
  : MessageParser(topic_name, data)
{
}

bool ROS2StringJsonMessageParser::parseRos2StringPayload(const PJ::MessageRef serialized_msg,
                                                         std::string& text) const
{
  const uint8_t* data = serialized_msg.data();
  const size_t size = serialized_msg.size();

  if (size < 8)
  {
    qWarning() << "ROS2 String message too short to parse";
    return false;
  }

  const uint32_t cdr_header = ReadLe32(data);
  if (cdr_header != 0x00010000 && cdr_header != 0x00000000)
  {
    qWarning() << "Unexpected CDR encapsulation for std_msgs/String:" << Qt::hex << cdr_header;
  }

  const uint32_t string_size = ReadLe32(data + 4);
  const size_t payload_end = size_t(8) + size_t(string_size);
  if (payload_end > size || string_size == 0)
  {
    qWarning() << "Invalid std_msgs/String payload size:" << string_size;
    return false;
  }

  const char* str_ptr = reinterpret_cast<const char*>(data + 8);
  if (str_ptr[string_size - 1] != '\0')
  {
    qWarning() << "std_msgs/String payload is not null-terminated";
    return false;
  }

  text.assign(str_ptr, str_ptr + string_size - 1);
  return true;
}

void ROS2StringJsonMessageParser::pushNumeric(const std::string& key, double timestamp, double value)
{
  const QString qkey = QString::fromStdString(key);
  if (!_known_series.contains(qkey))
  {
    if (_known_series.size() >= qsizetype(_max_series))
    {
      qWarning() << "Refusing to create additional JSON series beyond limit:" << _max_series
                 << "key=" << qkey;
      return;
    }
    _known_series.insert(qkey);
  }
  getSeries(key).pushBack({ timestamp, value });
}

void ROS2StringJsonMessageParser::flattenJson(const nlohmann::json& value, const std::string& prefix,
                                              double timestamp)
{
  if (value.is_object())
  {
    for (auto it = value.begin(); it != value.end(); ++it)
    {
      const std::string child_key = prefix.empty() ? it.key() : prefix + "." + it.key();
      flattenJson(it.value(), child_key, timestamp);
    }
    return;
  }

  if (value.is_number_integer())
  {
    pushNumeric(prefix, timestamp, static_cast<double>(value.get<int64_t>()));
    return;
  }

  if (value.is_number_unsigned())
  {
    pushNumeric(prefix, timestamp, static_cast<double>(value.get<uint64_t>()));
    return;
  }

  if (value.is_number_float())
  {
    pushNumeric(prefix, timestamp, value.get<double>());
    return;
  }
}

bool ROS2StringJsonMessageParser::parseMessage(const PJ::MessageRef serialized_msg, double& timestamp)
{
  std::string text;
  if (!parseRos2StringPayload(serialized_msg, text))
  {
    return false;
  }

  nlohmann::json value;
  try
  {
    value = nlohmann::json::parse(text);
  }
  catch (const std::exception& ex)
  {
    qWarning() << "Failed to parse JSON from std_msgs/String:" << ex.what();
    return false;
  }

  if (!value.is_object())
  {
    qWarning() << "Expected top-level JSON object in std_msgs/String";
    return false;
  }

  flattenJson(value, "", timestamp);
  return true;
}

PJ::MessageParserPtr ParserFactoryROS2StringJson::createParser(const std::string& topic_name,
                                                               const std::string& type_name,
                                                               const std::string& schema,
                                                               PJ::PlotDataMapRef& data)
{
  (void)schema;
  if (type_name != "std_msgs/msg/String" && type_name != "std_msgs/String")
  {
    throw std::runtime_error("ParserROS2StringJson only supports std_msgs/msg/String");
  }
  return std::make_shared<ROS2StringJsonMessageParser>(topic_name, data);
}
