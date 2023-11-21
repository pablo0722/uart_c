class Line {
    private:
        static constexpr int LINE_SIZE = 1024;

    private:
        int idx = 0;
        char string[LINE_SIZE];
        bool has_more_space = true;

    public:
        bool add_char(char c);
        bool is_empty();
        bool is_full();
        const char *pop();
};