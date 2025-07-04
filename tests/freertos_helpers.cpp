#include "freertos_helpers.hpp"

void stream_buffer_delete_clear_handle(
  StreamBufferHandle_t& stream_buffer_handle) {
  if (!stream_buffer_handle) return;
  vStreamBufferDelete(stream_buffer_handle);
  stream_buffer_handle = NULL;
}

void queue_delete_clear_handle(QueueHandle_t& queue_handle) {
  if (!queue_handle) return;
  vQueueDelete(queue_handle);
  queue_handle = NULL;
}
