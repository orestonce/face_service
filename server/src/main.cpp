#include "handle.h"

int main()
{
	httplib::Server server;
	server.Post("/facedetectcnn", handle_facedetectcnn);
	server.Post("/facedetectsf", handle_facedetectsf);
	server.Post("/facesearchsf", handle_facesearchsf);
	printf("start listen :8080\n");
	server.listen("0.0.0.0", 8080);
}

