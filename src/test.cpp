#include <unistd.h>
#include <fstream>
#include <chrono>
#include <thread>

#include "config.h"
#include "gmark.h"
#include "configparser.h"

int main(int argc, char** argv) {
    size_t node_count = 0;
	string config_file, output_file;
    int c;
    while ((c = getopt(argc, argv, "n:c:o:")) != -1) {
        switch(c) {
            case 'n':
                node_count = stol(optarg);
                break;
            case 'c':
                config_file = optarg;
                break;
            case 'o':
                output_file = optarg;
                break;
        }
    }
    config::config conf;
    conf.nb_nodes = node_count;
    conf.input = config_file;
    configparser::parse_config(conf);
    ofstream graph_stream;
    graph_stream.open(output_file);
    graph::ntriple_graph_writer writer(graph_stream);
    writer.build_graph(conf);
    graph_stream.close();
}