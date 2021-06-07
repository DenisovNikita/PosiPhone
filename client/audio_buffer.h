#ifndef POSIPHONE_AUDIO_BUFFER_H
#define POSIPHONE_AUDIO_BUFFER_H

#include <QBuffer>
#include <memory>
#include <mutex>
#include <vector>

namespace PosiPhone {
class AudioBuffer : public QBuffer {
    std::mutex mutex_;

public:
    explicit AudioBuffer(QObject *parent = nullptr);
    qint64 readData(char *data, qint64 maxSize) override;
    qint64 writeData(const char *data, qint64 maxSize) override;
    std::shared_ptr<std::vector<char>> read_and_clear();
    void clear();
    void clear_and_write(const std::shared_ptr<std::vector<char>> &ptr);
};

}  // namespace PosiPhone

#endif  // POSIPHONE_AUDIO_BUFFER_H
