

class Buffer
{
    private:
    GLuint id;
    GLenum target;
    size_t size;

public:
    Buffer(GLenum target) : target(target), size(0) {
        glGenBuffers(1, &id);
    }

    Buffer(const Buffer&) = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer(Buffer&& other) noexcept : id(other.id), target(other.target), size(other.size) {
        other.id = 0;
        other.size = 0;
    }

    ~Buffer() {
        if (id != 0) glDeleteBuffers(1, &id);
    }

    void setData(size_t size, const void* data, GLenum usage) {
        glBindBuffer(target, id);
        glBufferData(target, size, data, usage);
        this->size = size;
    }

    void updateData(size_t offset, size_t size, const void* data) {
        if (offset + size > this->size) return

        glBindBuffer(target, id);
        glBufferSubData(target, offset, size, data);
    }

    void bind() const {
        glBindBuffer(target, id);
    }

    void bindBase(GLuint binding) const {
        glBindBufferBase(target, binding, id);
    }

}