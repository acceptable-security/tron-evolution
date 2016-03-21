#include <math.h>
#include <stdlib.h>
#include "neural.h"

static unsigned int default_alloc = 8;

static inline double sigmoid(double x) {
    return 2 / (1 + exp(-4.9 * x)) - 1;
}

neural_network_t* neural_network_init(unsigned int input_node_cnt, unsigned int output_node_cnt) {
    // Look mom, no safety!
    neural_network_t* network = (neural_network_t*) malloc(sizeof(neural_network_t));

    network->input_nodes = (neural_node_t**) malloc(input_node_cnt * sizeof(neural_node_t*));
    network->input_node_cnt = input_node_cnt;

    network->output_nodes = (neural_node_t**) malloc(output_node_cnt * sizeof(neural_node_t*));
    network->output_node_cnt = output_node_cnt;

    network->nodes = (neural_node_t**) malloc(default_alloc * sizeof(neural_node_t*));
    network->node_cnt = 0;
    network->node_alloc = default_alloc;

    for ( int i = 0; i < input_node_cnt; i++ ) {
        network->input_nodes[i] = neural_node_init(0.0);
    }

    for ( int i = 0; i < input_node_cnt; i++ ) {
        network->output_nodes[i] = neural_node_init(0.0);
    }

    return network;
}

neural_node_t* neural_network_get_node(neural_network_t* network, int n) {
    if ( n > network->input_node_cnt + network->node_cnt + network->output_node_cnt ) {
        return NULL;
    }

    if ( n > network->input_node_cnt + network->node_cnt ) {
        return network->output_nodes[n - (network->input_node_cnt + network->node_cnt)];
    }

    if ( n > network->input_node_cnt ) {
        return network->nodes[n - network->input_node_cnt];
    }

    return network->input_nodes[n];
}

void neural_network_add_connection(neural_network_t* network, int input, int output, double weight) {
    neural_connection_t* connection = (neural_connection_t*) malloc(sizeof(neural_connection_t*));

    connection->input = neural_network_get_node(network, input);
    connection->output = neural_network_get_node(network, output);
    connection->weight = weight;

    neural_node_add_output(connection->input, connection);
    neural_node_add_input(connection->output, connection);
}

void neural_network_evaluate(neural_network_t* network) {
    // Assuming that inputs are set...

    for ( int i = 0; i < network->node_cnt; i++ ) {
        neural_node_t* node = network->nodes[i];
        double sum = 0;

        for ( int j = 0; j < node->input_cnt; j++ ) {
            neural_connection_t* conn = node->input[j];

            sum += node->weight * conn->weight * conn->input->value;
        }

        node->value = sigmoid(sum);
    }

    for ( int i = 0; i < network->output_node_cnt; i++ ) {
        neural_node_t* node = network->output_nodes[i];
        double sum = 0;

        for ( int j = 0; j < node->input_cnt; j++ ) {
            neural_connection_t* conn = node->input[j];

            sum += node->weight * conn->weight * conn->input->value;
        }

        node->value = sigmoid(sum);
    }
}

void neural_network_clean(neural_network_t* network) {
    // wew less safety
    for ( int i = 0; i < network->node_cnt; i++ ) {
        if ( network->nodes[i] != NULL ) {
            neural_node_clean(network->nodes[i]);
        }
    }

    free(network->nodes);
    free(network);
}

neural_node_t* neural_node_init(double weight) {
    neural_node_t* node = (neural_node_t*) malloc(sizeof(neural_node_t));

    node->input = (neural_connection_t**) malloc(default_alloc * sizeof(neural_connection_t*));
    node->input_cnt = 0;
    node->input_alloc = default_alloc;

    node->output = (neural_connection_t**) malloc(default_alloc * sizeof(neural_connection_t*));
    node->output_cnt = 0;
    node->output_alloc = default_alloc;

    node->weight = weight;

    return node;
}

void neural_node_add_input(neural_node_t* node, neural_connection_t* input) {
    if ( node == NULL ) {
        return;
    }

    if ( (node->input_cnt + 1) > node->input_alloc ) {
        node->input_alloc += default_alloc;

        neural_connection_t** tmp = realloc(node->input, node->input_alloc * sizeof(neural_connection_t*));

        if ( tmp == NULL ) {
            // shit
        }

        node->input = tmp;
    }

    node->input[node->input_cnt++] = input;
}

void neural_node_add_output(neural_node_t* node, neural_connection_t* output) {
    if ( node == NULL ) {
        return;
    }

    if ( (node->output_cnt + 1) > node->output_alloc ) {
        node->output_alloc += default_alloc;

        neural_connection_t** tmp = realloc(node->input, node->output_alloc * sizeof(neural_connection_t*));

        if ( tmp == NULL ) {
            // shit
        }

        node->output = tmp;
    }

    node->output[node->output_cnt++] = output;
}

void neural_node_clean(neural_node_t* node) {
    free(node->input);
    free(node->output);
    free(node);
}
