#include <microhttpd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PORT 8080

// Phương thức xử lý yêu cầu GET từ client
int handle_get_request(void *cls, struct MHD_Connection *connection, const char *url, const char *method,
                       const char *version, const char *upload_data, size_t *upload_data_size, void **con_cls)
{
    // Đọc nội dung tệp và gửi phản hồi tương ứng
    if (strcmp(url, "/") == 0) {
        // Yêu cầu trang chủ, trả về nội dung HTML
        const char *content = "<html><body><h1>Trang chủ</h1></body></html>";
        struct MHD_Response *response = MHD_create_response_from_buffer(strlen(content), (void *)content, MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);
        return ret;
    } else if (strcmp(url, "/image.jpg") == 0) {
        // Yêu cầu ảnh, trả về nội dung ảnh
        const char *image_path = "path/to/image.jpg";
        FILE *fp = fopen(image_path, "rb");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            long image_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            char *image_data = malloc(image_size);
            fread(image_data, 1, image_size, fp);
            fclose(fp);
            
            struct MHD_Response *response = MHD_create_response_from_buffer(image_size, (void *)image_data, MHD_RESPMEM_MUST_FREE);
            MHD_add_response_header(response, "Content-Type", "image/jpeg");
            int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);
            return ret;
        }
    } else if (strcmp(url, "/audio.mp3") == 0) {
        // Yêu cầu âm thanh, trả về nội dung âm thanh
        const char *audio_path = "path/to/audio.mp3";
        FILE *fp = fopen(audio_path, "rb");
        if (fp) {
            fseek(fp, 0, SEEK_END);
            long audio_size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            char *audio_data = malloc(audio_size);
            fread(audio_data, 1, audio_size, fp);
            fclose(fp);
            
            struct MHD_Response *response = MHD_create_response_from_buffer(audio_size, (void *)audio_data, MHD_RESPMEM_MUST_FREE);
            MHD_add_response_header(response, "Content-Type", "audio/mpeg");
            int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);
            return ret;
        }
    }
    
    // Yêu cầu không hợp lệ, trả về lỗi 404 Not Found
    const char *error_message = "404 Not Found";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_message), (void *)error_message, MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response);
    MHD_destroy_response(response);
    return ret;
}

int main()
{
    struct MHD_Daemon *daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, PORT, NULL, NULL,
                                                &handle_get_request, NULL, MHD_OPTION_END);
    if (!daemon) {
        printf("Không thể khởi động HTTP server\n");
        return 1;
    }
    
    printf("HTTP server đang chạy trên cổng %d...\n", PORT);
    
    getchar(); // Đợi người dùng nhấn Enter để dừng server
    
    MHD_stop_daemon(daemon);
    return 0;
}
