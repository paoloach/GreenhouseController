//
// Created by paolo on 12/9/20.
//

#ifndef GREENHOUSECONTROLLER_WEBSERVER_H
#define GREENHOUSECONTROLLER_WEBSERVER_H


#ifdef __cplusplus

class WebServer{
    httpd_handle_t httpServer;
public:
    static constexpr const char * TAG="WebServer";
    void init();
private:
    static esp_err_t aboutHandler(httpd_req_t *req);
    static esp_err_t getStatusHandler(httpd_req_t *req);
    static esp_err_t postOTA(httpd_req_t *req);
    static esp_err_t setSampleIntervalHandler(httpd_req_t *req);
};

extern WebServer webServer;



extern "C" {
    void initWebserver(void);
};

#else
void initWebserver(void);
#endif

#endif //GREENHOUSECONTROLLER_WEBSERVER_H
