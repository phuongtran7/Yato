﻿#include "Yato.h"

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

int main()
{
	YAML::Emitter out;

	auto config = get_config_file_path();

	const auto input_file = cpptoml::parse_file(config.generic_string());
	auto address = input_file->get_as<std::string>("address").value_or("tcp://192.168.72.249:1883");

	out << YAML::BeginDoc;

	out << YAML::BeginMap;
	out << YAML::Key << "Address";
	out << YAML::Value << address;
	out << YAML::EndMap;

	// Get the publishing topics
	auto publish_topics = input_file->get_array_of<std::string>("publish_topic");
	if (publish_topics) {
		out << YAML::BeginMap;
		out << YAML::Key << "Publish Topic";

		out << YAML::Value;
		out << YAML::BeginSeq;
		for (auto&& name : *publish_topics) {
			out << name;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	// Get the subscribing topics
	auto subscribe_topics = input_file->get_array_of<std::string>("subscribe_topic");
	if (subscribe_topics) {
		out << YAML::BeginMap;
		out << YAML::Key << "Subscirbe Topic";

		out << YAML::Value;
		out << YAML::BeginSeq;
		for (auto&& name : *subscribe_topics) {
			out << name;
		}
		out << YAML::EndSeq;
		out << YAML::EndMap;
	}

	for (const auto& topic : *publish_topics)
	{
		out << YAML::BeginMap;
		out << YAML::Key << topic;
		out << YAML::Value;

		out << YAML::BeginSeq;
		const auto data_list = input_file->get_table_array(topic);
		if (data_list != nullptr) {
			// Loop through all the tables
			for (const auto& table : *data_list) {
				out << YAML::BeginMap;
				out << YAML::Key << table->get_as<std::string>("name").value_or("");
				out << YAML::Value;

				out << YAML::BeginMap;
				out << YAML::Key << "dataref" << YAML::Value << table->get_as<std::string>("string").value_or("");
				out << YAML::Key << "type" << YAML::Value << table->get_as<std::string>("type").value_or("");

				auto start = table->get_as<int>("start_index").value_or(-1);
				if (start != -1) {
					out << YAML::Key << "start" << YAML::Value << start;
				}

				auto num = table->get_as<int>("num_value").value_or(-1);
				if (num != -1) {
					out << YAML::Key << "end" << YAML::Value << start;
				}
				out << YAML::EndMap;
				out << YAML::EndMap;
			}
		}
		out << YAML::EndSeq;
	}

	out << YAML::EndDoc;

	fmt::print("Output:\n\n {}\n", out.c_str());

	std::ofstream outfile;
	auto out_file = fmt::format("{}.yaml", config.stem().generic_string());
	outfile.open(out_file, std::ios_base::out);
	outfile << out.c_str();

	std::getchar();
	return 0;
}
