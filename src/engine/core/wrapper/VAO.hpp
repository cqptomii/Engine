

class VAO
{
    private:
        GLuint vao_id;

    public:
        VAO() : vao_id(0) {
            glGenVertexArrays(1, &vao_id);
        }

        ~VAO() {
            if (vao_id != 0) {
                glDeleteVertexArrays(1, &vao_id);
            }
        }

        void set_vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* offset) {
            glBindVertexArray(vao_id);
            glVertexAttribPointer(index, size, type, normalized, stride, pointer);
            glEnableVertexAttribArray(index);
            glBindVertexArray(0);
        }

        void bind() const {
            glBindVertexArray(vao_id);
        }

        void unbind() const {
            glBindVertexArray(0);
        }
}