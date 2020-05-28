#include "Yato.h"

std::filesystem::path get_config_file_path() {
	auto current_path = std::filesystem::current_path();
	for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
		// Look for ".toml" file
		auto ext = entry.path().extension().generic_string();
		if (ext == ".toml") {
			return entry.path();
		}
	}
	return std::filesystem::path{};
}

void write_topic(YAML::Emitter& emitter, cpptoml::option<std::vector<std::string>>& topic, const std::string& name) {
	emitter << YAML::BeginMap;

	emitter << YAML::Key << name;
	emitter << YAML::Value;
	emitter << YAML::BeginSeq;
	for (auto&& name : *topic) {
		emitter << YAML::DoubleQuoted << name;
	}
	emitter << YAML::EndSeq;

	emitter << YAML::EndMap;
	emitter << YAML::Newline;
}

void write_to_file(std::string_view output , std::string_view file_name) {
	std::ofstream outfile;
	outfile.open(file_name, std::ios_base::out);
	outfile << output;
	outfile.close();
}

void write_dataref_in_topic(YAML::Emitter& emitter, cpptoml::option<std::vector<std::string>>& topic, std::shared_ptr<cpptoml::table> input_file) {
	for (auto&& topic : *topic)
	{
		emitter << YAML::BeginMap;

		emitter << YAML::Key << topic;
		emitter << YAML::Value;
		emitter << YAML::BeginSeq;
		auto data_list = input_file->get_table_array(topic);
		if (data_list) {
			// Loop through all the tables
			for (const auto& table : *data_list) {
				emitter << YAML::BeginMap;
				emitter << YAML::Key << YAML::DoubleQuoted << table->get_as<std::string>("name").value_or("");
				emitter << YAML::Value;

				emitter << YAML::BeginMap;
				emitter << YAML::Key << "dataref" << YAML::Value << YAML::DoubleQuoted << table->get_as<std::string>("string").value_or("");
				emitter << YAML::Key << "type" << YAML::Value << YAML::DoubleQuoted << table->get_as<std::string>("type").value_or("");

				auto start = table->get_as<int>("start_index").value_or(-1);
				if (start != -1) {
					emitter << YAML::Key << "start" << YAML::Value << YAML::DoubleQuoted << start;
				}

				auto num = table->get_as<int>("num_value").value_or(-1);
				if (num != -1) {
					emitter << YAML::Key << "num_value" << YAML::Value << YAML::DoubleQuoted << num;
				}
				emitter << YAML::EndMap;
				emitter << YAML::EndMap;
				emitter << YAML::Newline;
			}
		}
		emitter << YAML::EndSeq;
		emitter << YAML::EndMap;
		emitter << YAML::Newline;
	}
}

int main()
{
	bool has_publish{ false };
	bool has_subscribe{ false };

	auto config = get_config_file_path();

	const auto input_file = cpptoml::parse_file(config.generic_string());
	auto address = input_file->get_as<std::string>("address").value_or("tcp://192.168.72.249:1883");

	YAML::Emitter out;
	out << YAML::BeginDoc;

	out << YAML::BeginMap;
	out << YAML::Key << "Address";
	out << YAML::Value << YAML::DoubleQuoted << address ;
	out << YAML::EndMap;
	out << YAML::Newline;

	// Get the publishing topics
	auto publish_topics = input_file->get_array_of<std::string>("publish_topic");
	if (publish_topics) {
		write_topic(out, publish_topics, "Publish Topic");
		has_publish = true;
	}

	// Get the subscribing topics
	auto subscribe_topics = input_file->get_array_of<std::string>("subscribe_topic");
	if (subscribe_topics) {
		write_topic(out, publish_topics, "Subscirbe Topic");
		has_subscribe = true;
	}

	if (has_publish) {
		write_dataref_in_topic(out, publish_topics, input_file);
	}
	
	if (has_subscribe) {
		write_dataref_in_topic(out, subscribe_topics, input_file);
	}

	out << YAML::EndDoc;

	//fmt::print("{}\n", out.c_str());

	auto out_file = fmt::format("{}.yaml", config.stem().generic_string());
	write_to_file(out.c_str(), out_file);

	std::getchar();
	return 0;
}
