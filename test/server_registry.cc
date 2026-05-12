#include <server/rpc_server.hpp>

int main() {
    gchrpc::server::RegistryServer registry_server(9090);
    registry_server.Start();
}
