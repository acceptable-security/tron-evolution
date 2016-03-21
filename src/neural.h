
#include <stdbool.h>

typedef struct _neural_connection neural_connection_t;

typedef struct _neural_node {
    neural_connection_t** input;
    unsigned int input_cnt;
    unsigned int input_alloc;

    neural_connection_t** output;
    unsigned int output_cnt;
    unsigned int output_alloc;

    double weight;
    double value;
} neural_node_t;

struct _neural_connection {
    neural_node_t* input;
    neural_node_t* output;
    double weight;
};

typedef struct {
    neural_node_t** input_nodes;
    unsigned int input_node_cnt;

    neural_node_t** output_nodes;
    unsigned int output_node_cnt;

    neural_node_t** nodes;
    unsigned int node_cnt;
    unsigned int node_alloc;
} neural_network_t;

neural_network_t* neural_network_init(unsigned int input_node_cnt, unsigned int output_node_cnt);
void neural_network_add_connection(neural_network_t* network, int input, int output, double weight);
void neural_network_evaluate(neural_network_t* network);
void neural_network_clean(neural_network_t* network);

neural_node_t* neural_node_init(double weight);
void neural_node_add_input(neural_node_t* node, neural_connection_t* input);
void neural_node_add_output(neural_node_t* node, neural_connection_t* output);
void neural_node_clean(neural_node_t* node);
