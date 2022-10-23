#include "client.hpp"
#include "api/api.hpp"
#include "client/scenes/starter.hpp"

int32_t main(int32_t, char**) {
	api::mainloop(new client::scenes::starter());
	return 0;
}