#pragma once

#include <PlotJuggler/messageparser_base.h>

#include <nlohmann/json.hpp>

#include <QSet>
#include <QString>

class ROS2StringJsonMessageParser : public PJ::MessageParser
{
public:
  ROS2StringJsonMessageParser(const std::string& topic_name, PJ::PlotDataMapRef& data);

  bool parseMessage(const PJ::MessageRef serialized_msg, double& timestamp) override;

private:
  bool parseRos2StringPayload(const PJ::MessageRef serialized_msg, std::string& text) const;
  void flattenJson(const nlohmann::json& value, const std::string& prefix, double timestamp);
  void pushNumeric(const std::string& key, double timestamp, double value);

  size_t _max_series = 200;
  QSet<QString> _known_series;
};

class ParserFactoryROS2StringJson : public PJ::ParserFactoryPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "facontidavide.PlotJuggler3.ParserFactoryPlugin")
  Q_INTERFACES(PJ::ParserFactoryPlugin)

public:
  ParserFactoryROS2StringJson() = default;

  const char* name() const override
  {
    return "ParserFactoryROS2StringJson";
  }

  const char* encoding() const override
  {
    return "ros2msg_string_json";
  }

  PJ::MessageParserPtr createParser(const std::string& topic_name, const std::string& type_name,
                                    const std::string& schema, PJ::PlotDataMapRef& data) override;
};
