#include <stdint.h>
#include <stddef.h>
#include <vector>

typedef bool bit;

class BitString;

void load_into_bit_string(BitString &, uint8_t, uint8_t);
void load_into_bit_string(BitString &, const char *, uint64_t, uint8_t);

class BitString
{
private:
    uint8_t *string;
    uint8_t padding;
    uint64_t byte_size;
    uint64_t byte_pos = 0;
    uint8_t bit_pos = 0;

    void init();
    void reinit();

public:
    BitString();

    void reset_pos();
    size_t get_bit_size() const;
    uint64_t get_size() const;
    const uint8_t *get_string() const;
    BitString get_last_byte() const;
    uint8_t get_byte(uint64_t) const;
    uint8_t get_byte_padding() const;

    bool operator<<(uint8_t);
    bool operator>>(bit &);
    uint8_t operator>>(uint8_t &);
    bool operator==(BitString &);
    bool operator!=(BitString &);
    BitString operator+(BitString &);

    friend void load_into_bit_string(BitString &, uint8_t, uint8_t);
    friend void load_into_bit_string(BitString &, const char *, uint64_t, uint8_t);
};

template <typename T>
void sort(std::vector<T> &);

template <typename T>
void sort_ptr(std::vector<T> &);

typedef BitString HuffmanPath;

namespace Tree
{
    class HuffmanTree;

    BitString huffman_exceptions_to_bit_string(Tree::HuffmanTree &);

    class HuffmanNode
    {
    private:
        uint64_t frq;
        Tree::HuffmanNode *left_node = NULL, *right_node = NULL, *parent_node = NULL;

    public:
        HuffmanNode(uint64_t);
        HuffmanNode(Tree::HuffmanNode *, Tree::HuffmanNode *);

        bool has_value() const;
        HuffmanPath get_path() const;
        uint64_t get_frq() const;
        Tree::HuffmanNode *get_left_node() const;
        Tree::HuffmanNode *get_right_node() const;
        Tree::HuffmanNode *get_parent_node() const;

        bool operator<(Tree::HuffmanNode) const;
        bool operator>(Tree::HuffmanNode) const;

        friend BitString Tree::huffman_exceptions_to_bit_string(Tree::HuffmanTree &);
    };

    struct Key
    {
        uint8_t chr;
        uint64_t frq;
    };

    struct HuffmanKey
    {
        Tree::Key key;
        Tree::HuffmanNode *node;
    };

    class HuffmanTree
    {
    private:
        std::vector<Tree::HuffmanNode *> queue;
        std::vector<Tree::HuffmanNode *> huffman_nodes;
        std::vector<Tree::HuffmanNode *> exception_nodes;
        std::vector<Tree::HuffmanKey> huffman_keys;
        Tree::HuffmanNode *root_node = NULL;

    public:
        HuffmanTree(std::vector<Key>);

        void create_tree();

        std::vector<Tree::HuffmanKey> get_keys() const;
        std::vector<Tree::HuffmanNode *> get_excetion_nodes() const;
        Tree::HuffmanNode *get_root_node() const;

        friend BitString Tree::huffman_exceptions_to_bit_string(Tree::HuffmanTree &);
    };
}

namespace VirtualTree
{
    class HuffmanTree;

    void load_huffman_bit_string_exception_into_virtual_tree(VirtualTree::HuffmanTree &, BitString &);

    struct Key
    {
        uint8_t chr;
    };

    class HuffmanNode
    {
    private:
        VirtualTree::HuffmanNode *left_node = NULL, *right_node = NULL, *parent_node = NULL;
        bool has_exc = false;
        
    public:

        HuffmanNode();
        HuffmanNode(VirtualTree::HuffmanNode *, VirtualTree::HuffmanNode *);
        void exception();
        bool has_value() const;
        bool has_exception() const;
        HuffmanPath get_path() const;
        VirtualTree::HuffmanNode *get_left_node() const;
        VirtualTree::HuffmanNode *get_right_node() const;
        VirtualTree::HuffmanNode *get_parent_node() const;
    };

    struct HuffmanKey
    {
        VirtualTree::Key key;
        VirtualTree::HuffmanNode *node;
    };

    class HuffmanTree
    {
    private:
        std::vector<VirtualTree::HuffmanNode *> queue;
        std::vector<VirtualTree::HuffmanNode *> huffman_nodes;
        std::vector<VirtualTree::HuffmanNode *> exception_nodes;
        std::vector<VirtualTree::HuffmanKey> huffman_keys;
        VirtualTree::HuffmanNode *root_node;

    public:
        HuffmanTree(std::vector<VirtualTree::Key>);

        void create_tree();
        std::vector<VirtualTree::HuffmanKey> get_keys() const;
        VirtualTree::HuffmanNode *get_root_node() const;

        friend void VirtualTree::load_huffman_bit_string_exception_into_virtual_tree(VirtualTree::HuffmanTree &, BitString &);
    };
}

BitString encode(BitString &, Tree::HuffmanTree &);
BitString decode(BitString &, VirtualTree::HuffmanTree &);