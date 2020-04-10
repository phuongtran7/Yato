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
		out << YAML::BeginMap;

		out << YAML::Key << "Publish Topic";
		out << YAML::Value;
		out << YAML::BeginSeq;
		for (auto&& name : *publish_topics) {
			out << YAML::DoubleQuoted << name;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
		out << YAML::Newline;

		has_publish = true;
	}

	// Get the subscribing topics
	auto subscribe_topics = input_file->get_array_of<std::string>("subscribe_topic");
	if (subscribe_topics) {
		out << YAML::BeginMap;

		out << YAML::Key << "Subscirbe Topic";
		out << YAML::Value;
		out << YAML::BeginSeq;
		for (auto&& name : *subscribe_topics) {
			out << YAML::DoubleQuoted << name;
		}
		out << YAML::EndSeq;

		out << YAML::EndMap;
		out << YAML::Newline;

		has_subscribe = true;
	}

	if (has_publish) {
		for (auto&& topic : *publish_topics)
		{
			out << YAML::BeginMap;

			out << YAML::Key << topic;
			out << YAML::Value;
			out << YAML::BeginSeq;
			auto data_list = input_file->get_table_array(topic);
			if (data_list) {
				// Loop through all the tables
				for (const auto& table : *data_list) {
					out << YAML::BeginMap;
					out << YAML::Key << table->get_as<std::string>("name").value_or("");
					out << YAML::Value;

					out << YAML::BeginMap;
					out << YAML::Key << "dataref" << YAML::Value << YAML::DoubleQuoted << table->get_as<std::string>("string").value_or("");
					out << YAML::Key << "type" << YAML::Value << YAML::DoubleQuoted << table->get_as<std::string>("type").value_or("");

					auto start = table->get_as<int>("start_index").value_or(-1);
					if (start != -1) {
						out << YAML::Key << "start" << YAML::Value << YAML::DoubleQuoted << start;
					}

					auto num = table->get_as<int>("num_value").value_or(-1);
					if (num != -1) {
						out << YAML::Key << "end" << YAML::Value << YAML::DoubleQuoted << start;
					}
					out << YAML::EndMap;
					out << YAML::EndMap;
					out << YAML::Newline;
				}
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
			out << YAML::Newline;
		}
	}
	
	if (has_subscribe) {
		for (auto&& topic : *subscribe_topics)
		{
			out << YAML::BeginMap;

			out << YAML::Key << topic;
			out << YAML::Value;
			out << YAML::BeginSeq;
			auto data_list = input_file->get_table_array(topic);
			if (data_list) {
				// Loop through all the tables
				for (const auto& table : *data_list) {
					out << YAML::BeginMap;
					out << YAML::Key << table->get_as<std::string>("name").value_or("");
					out << YAML::Value;

					out << YAML::BeginMap;
					out << YAML::Key << "dataref" << YAML::Value << YAML::DoubleQuoted << table->get_as<std::string>("string").value_or("");
					out << YAML::Key << "type" << YAML::Value << YAML::DoubleQuoted << table->get_as<std::string>("type").value_or("");

					auto start = table->get_as<int>("start_index").value_or(-1);
					if (start != -1) {
						out << YAML::Key << "start" << YAML::Value << YAML::DoubleQuoted << start;
					}

					auto num = table->get_as<int>("num_value").value_or(-1);
					if (num != -1) {
						out << YAML::Key << "end" << YAML::Value << YAML::DoubleQuoted << start;
					}
					out << YAML::EndMap;
					out << YAML::EndMap;
					out << YAML::Newline;
				}
			}
			out << YAML::EndSeq;
			out << YAML::EndMap;
			out << YAML::Newline;
		}
	}

	out << YAML::EndDoc;

	fmt::print("{}\n", out.c_str());

	std::ofstream outfile;
	auto out_file = fmt::format("{}.yaml", config.stem().generic_string());
	outfile.open(out_file, std::ios_base::out);
	outfile << out.c_str();

	std::getchar();
	return 0;
}
