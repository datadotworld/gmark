#include <unistd.h>
#include <fstream>
#include <chrono>
#include <thread>

#include "config.h"
#include "gmark.h"
#include "configparser.h"

int main(int argc, char** argv) {
    size_t nb_nodes = 0;
	string conf_file = "../use-cases/test.xml";
    string graph_file;
    int c;
    while ((c = getopt(argc, argv, "n:c:g:")) != -1) {
        switch(c) {
            case 'n':
                nb_nodes = stol(optarg);
                break;
            case 'c':
                conf_file = optarg;
                break;
            case 'g':
                graph_file = optarg;
                break;
        }
    }
    config::config conf;
    conf.nb_nodes = nb_nodes;
    configparser::parse_config(conf_file, conf);
    ofstream graph_stream;
    string fileName = graph_file + ".dat";
    graph_stream.open(fileName);
    graph::ntriple_graph_writer writer(graph_stream);
    writer.build_graph(conf);
    graph_stream.close();
}