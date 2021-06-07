#include "audio_buffer.h"

namespace PosiPhone {
AudioBuffer::AudioBuffer(QObject *parent) : QBuffer(parent) {
}

qint64 AudioBuffer::readData(char *data, qint64 maxSize) {
    std::unique_lock lock(mutex_);
    return QBuffer::readData(data, maxSize);
}

qint64 AudioBuffer::writeData(const char *data, qint64 maxSize) {
    std::unique_lock lock(mutex_);
    return QBuffer::writeData(data, maxSize);
}

std::shared_ptr<std::vector<char>> AudioBuffer::read_and_clear() {
    std::unique_lock lock(mutex_);
    std::vector<char> tmp;
    tmp.assign(buffer().data(), buffer().data() + buffer().size());
    buffer().clear();
    reset();
    return std::make_shared<std::vector<char>>(tmp);
}

void AudioBuffer::clear() {
    std::unique_lock lock(mutex_);
    buffer().clear();
    reset();
}

void AudioBuffer::clear_and_write(
    const std::shared_ptr<std::vector<char>> &ptr) {
    std::unique_lock lock(mutex_);
    buffer().clear();
    reset();
    if (ptr) {
        buffer().append(ptr->data(), static_cast<int>(ptr->size()));
    }
}

}  // namespace PosiPhone