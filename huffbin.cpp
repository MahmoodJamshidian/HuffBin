#include "huffbin.hpp"
#include <stdexcept>
#include <iostream>

void BitString::init()
{
    this->string = (uint8_t *)malloc(sizeof(uint8_t) * this->byte_size);
    this->padding = 8;
}

void BitString::reinit()
{
    this->reset_pos();
    this->string = (uint8_t *)realloc(this->string, sizeof(uint8_t) * this->byte_size);
    this->padding = 8;
}

BitString::BitString()
{
    this->byte_size = 1;

    this->init();
}

void BitString::reset_pos()
{
    this->bit_pos = 0;
    this->byte_pos = 0;
}

uint64_t BitString::get_bit_size() const
{
    return (this->byte_size * 8) - this->padding;
}

uint64_t BitString::get_size() const
{
    return this->byte_size;
}

const uint8_t *BitString::get_string() const
{
    return (const uint8_t *)this->string;
}

BitString BitString::get_last_byte() const
{
    BitString res;

    *res.string = *(this->string + this->byte_size - 1);
    res.padding = this->padding;

    return res;
}

uint8_t BitString::get_byte(uint64_t offset) const
{
    if (offset > this->byte_size - 1)
    {
        throw std::runtime_error("offset so larg");
    }

    return *(this->string + offset);
}

uint8_t BitString::get_byte_padding() const
{
    return this->padding;
}

bool BitString::operator<<(uint8_t val)
{
    bool has_realloc = false;

    if (!this->padding)
    {
        this->byte_size++;
        this->string = (uint8_t *)realloc(this->string, sizeof(uint8_t) * this->byte_size);
        this->padding = 8;
        has_realloc = true;
    }

    this->padding--;

    *(this->string + this->byte_size - 1) <<= 1;

    if (val)
    {
        (*(this->string + this->byte_size - 1))++;
    }

    return has_realloc;
}

bool BitString::operator>>(bit &dst)
{
    bool end_of_string = false;

    if (this->byte_pos == this->byte_size - 1 && 8 - this->padding - this->bit_pos == 0)
    {
        throw std::overflow_error("end of string");
    }

    if (this->byte_pos == this->byte_size - 1 && 7 - this->padding - this->bit_pos == 0)
    {
        end_of_string = true;
    }

    if (this->bit_pos > 7)
    {
        this->bit_pos = 0;
        this->byte_pos++;
    }

    if (this->byte_pos == this->byte_size - 1)
    {
        dst = (uint8_t)((uint8_t)((*(this->string + this->byte_pos) << this->padding) << this->bit_pos) >> 7);
    }
    else
    {
        dst = (uint8_t)((uint8_t)(*(this->string + this->byte_pos) << this->bit_pos) >> 7);
    }

    this->bit_pos++;

    return end_of_string;
}

uint8_t BitString::operator>>(uint8_t &dst)
{
    uint8_t res = 0;

    if (this->byte_pos == this->byte_size)
    {
        this->byte_pos = 0;
        res = this->padding;
    }

    dst = *(this->string + this->byte_pos++);

    return res;
}

bool BitString::operator==(BitString &other)
{
    if (this->byte_size != other.byte_size || this->padding != other.padding)
    {
        return false;
    }

    for (uint64_t ind = 0; ind < this->byte_size; ind++)
    {
        if (*(this->string + ind) != *(other.string + ind))
        {
            return false;
        }
    }

    return true;
}

bool BitString::operator!=(BitString &other)
{
    return this->operator==(other);
}

template <typename T>
void sort(std::vector<T> &arr)
{
    T loc_data;

    for (uint64_t i = 0; i < arr.size(); i++)
    {
        for (uint64_t j = 0, k = 1; k < arr.size(); j++, k++)
        {
            if (arr.at(j) > arr.at(k))
            {
                loc_data = arr.at(j);
                arr.at(j) = arr.at(k);
                arr.at(k) = loc_data;
            }
        }
    }
}

BitString BitString::operator+(BitString &other)
{
    BitString res = *this;
    uint64_t d_byte_pos = other.byte_pos;
    uint64_t d_bit_pos = other.bit_pos;
    bit val;

    if (other.get_bit_size() < 1)
    {
        return res;
    }

    other.reset_pos();

    while (!other.operator>>(val))
    {
        res << val;
    }

    res << val;

    other.byte_pos = d_byte_pos;
    other.bit_pos = d_bit_pos;

    return res;
}

void load_into_bit_string(BitString &bs, uint8_t data, uint8_t padding)
{
    bs.byte_size = 1;

    bs.reinit();

    *bs.string = data >> padding;
    bs.padding = padding;
}

void load_into_bit_string(BitString &bs, const char *data, uint64_t size, uint8_t padding)
{
    if (size < 1)
    {
        throw std::runtime_error("invalid size");
    }

    bs.byte_size = size;

    bs.reinit();

    bs.padding = padding;

    uint64_t ind = 0;

    for (; ind < size - 1; ind++)
    {
        *(bs.string + ind) = *(data + ind);
    }

    *(bs.string + ind) = *(data + ind) >> padding;
}

template <typename T>
void sort_ptr(std::vector<T> &arr)
{
    T loc_data;

    for (uint64_t i = 0; i < arr.size(); i++)
    {
        for (uint64_t j = 0, k = 1; k < arr.size(); j++, k++)
        {
            if (*(arr.at(j)) > *(arr.at(k)))
            {
                loc_data = arr.at(j);
                arr.at(j) = arr.at(k);
                arr.at(k) = loc_data;
            }
        }
    }
}

Tree::HuffmanNode::HuffmanNode(uint64_t frq) : frq(frq) {}

Tree::HuffmanNode::HuffmanNode(Tree::HuffmanNode *left_node, Tree::HuffmanNode *right_node) : left_node(left_node), right_node(right_node)
{
    this->left_node->parent_node = this;
    this->right_node->parent_node = this;
    this->frq = this->left_node->get_frq() + this->right_node->get_frq();
}

bool Tree::HuffmanNode::has_value() const
{
    return !(this->left_node && this->right_node);
}

HuffmanPath Tree::HuffmanNode::get_path() const
{
    if (this->parent_node == NULL)
    {
        return HuffmanPath();
    }

    HuffmanPath parent_path = this->parent_node->get_path();
    HuffmanPath path;

    path = parent_path;

    if (this->parent_node->right_node == this)
    {
        path << 1;
    }
    else
    {
        path << 0;
    }

    return path;
}

uint64_t Tree::HuffmanNode::get_frq() const
{
    return this->frq;
}

Tree::HuffmanNode *Tree::HuffmanNode::get_left_node() const
{
    return this->left_node;
}

Tree::HuffmanNode *Tree::HuffmanNode::get_right_node() const
{
    return this->right_node;
}

Tree::HuffmanNode *Tree::HuffmanNode::get_parent_node() const
{
    return this->parent_node;
}

bool Tree::HuffmanNode::operator<(Tree::HuffmanNode other) const
{
    return this->frq < other.frq;
}

bool Tree::HuffmanNode::operator>(Tree::HuffmanNode other) const
{
    return this->frq > other.frq;
}

Tree::HuffmanTree::HuffmanTree(std::vector<Tree::Key> keys)
{
    Tree::Key loc_key;
    Tree::HuffmanNode *loc_node;

    for (uint64_t ind = 0; ind < keys.size(); ind++)
    {
        loc_key = keys.at(ind);

        loc_node = new Tree::HuffmanNode(loc_key.frq);

        this->huffman_keys.push_back({loc_key, loc_node});
        queue.push_back(loc_node);
    }
}

void Tree::HuffmanTree::create_tree()
{
    Tree::HuffmanNode *loc_node;
    bool exc_node = false;

    sort_ptr<Tree::HuffmanNode *>(this->queue);

    while (this->queue.size() > 1)
    {
        std::vector<Tree::HuffmanNode *> n_queue;

        loc_node = new Tree::HuffmanNode(this->queue.at(0), this->queue.at(1));

        n_queue.push_back(loc_node);

        if (exc_node)
        {
            this->exception_nodes.push_back(loc_node);
            exc_node = false;
        }

        for (uint64_t i = 2; i < this->queue.size(); i++)
        {
            n_queue.push_back(this->queue.at(i));
        }

        sort_ptr<Tree::HuffmanNode *>(n_queue);

        if (n_queue.size() > 1 && !n_queue.at(1)->has_value())
        {
            exc_node = true;
        }

        this->queue.swap(n_queue);
    }

    this->root_node = loc_node;
}

std::vector<Tree::HuffmanKey> Tree::HuffmanTree::get_keys() const
{
    return this->huffman_keys;
}

std::vector<Tree::HuffmanNode *> Tree::HuffmanTree::get_excetion_nodes() const
{
    return this->exception_nodes;
}

Tree::HuffmanNode *Tree::HuffmanTree::get_root_node() const
{
    return this->root_node;
}

BitString Tree::huffman_exceptions_to_bit_string(Tree::HuffmanTree &hft)
{
    BitString res;

    Tree::HuffmanNode *root_node = hft.get_root_node();
    Tree::HuffmanNode *ptr_node = root_node;

    bool is_exc = false;
    uint64_t c_exc = 0;
    uint64_t l_exc = hft.exception_nodes.size();

    while (c_exc < l_exc && !ptr_node->has_value())
    {
        is_exc = false;

        for (uint64_t ind = 0; ind < l_exc; ind++)
        {
            if (ptr_node == hft.exception_nodes.at(ind))
            {
                res << 1;

                ptr_node = ptr_node->get_right_node();
                is_exc = true;
                c_exc++;

                break;
            }
        }

        if (!is_exc)
        {
            res << 0;

            ptr_node = ptr_node->get_left_node();
        }
    }

    return res;
}

VirtualTree::HuffmanNode::HuffmanNode() {}

VirtualTree::HuffmanNode::HuffmanNode(VirtualTree::HuffmanNode *left_node, VirtualTree::HuffmanNode *right_node) : left_node(left_node), right_node(right_node)
{
    this->left_node->parent_node = this;
    this->right_node->parent_node = this;
}

void VirtualTree::HuffmanNode::exception()
{
    if (this->has_exc)
    {
        throw std::runtime_error("exception repeat");
    }
    if (this->has_value())
    {
        throw std::runtime_error("this node has value");
    }

    VirtualTree::HuffmanNode *loc_node = this->left_node;

    this->left_node = this->right_node;
    this->right_node = loc_node;

    this->has_exc = true;
}

bool VirtualTree::HuffmanNode::has_value() const
{
    return !(this->left_node && this->right_node);
}

bool VirtualTree::HuffmanNode::has_exception() const
{
    return this->has_exc;
}

HuffmanPath VirtualTree::HuffmanNode::get_path() const
{

    if (this->parent_node == NULL)
    {
        return HuffmanPath();
    }

    HuffmanPath parent_path = this->parent_node->get_path();
    HuffmanPath path;

    path = parent_path;

    if (this->parent_node->right_node == this)
    {
        path << 1;
    }
    else
    {
        path << 0;
    }

    return path;
}

VirtualTree::HuffmanNode *VirtualTree::HuffmanNode::get_left_node() const
{
    return this->left_node;
}

VirtualTree::HuffmanNode *VirtualTree::HuffmanNode::get_right_node() const
{
    return this->right_node;
}

VirtualTree::HuffmanNode *VirtualTree::HuffmanNode::get_parent_node() const
{
    return this->parent_node;
}

VirtualTree::HuffmanTree::HuffmanTree(std::vector<VirtualTree::Key> keys)
{
    VirtualTree::Key loc_key;
    VirtualTree::HuffmanNode *loc_node;

    for (uint64_t ind = 0; ind < keys.size(); ind++)
    {
        loc_key = keys.at(ind);

        loc_node = new VirtualTree::HuffmanNode();

        this->huffman_keys.push_back({loc_key, loc_node});
        queue.push_back(loc_node);
    }
}

void VirtualTree::HuffmanTree::create_tree()
{
    VirtualTree::HuffmanNode *loc_node;

    while (this->queue.size() > 1)
    {
        std::vector<VirtualTree::HuffmanNode *> n_queue;

        loc_node = new VirtualTree::HuffmanNode(this->queue.at(0), this->queue.at(1));

        n_queue.push_back(loc_node);

        for (uint64_t i = 2; i < this->queue.size(); i++)
        {
            n_queue.push_back(this->queue.at(i));
        }

        this->queue.swap(n_queue);
    }

    this->root_node = loc_node;
}

std::vector<VirtualTree::HuffmanKey> VirtualTree::HuffmanTree::get_keys() const
{
    return this->huffman_keys;
}

VirtualTree::HuffmanNode *VirtualTree::HuffmanTree::get_root_node() const
{
    return this->root_node;
}

void VirtualTree::load_huffman_bit_string_exception_into_virtual_tree(VirtualTree::HuffmanTree &hft, BitString &excs)
{
    bool bit;
    uint64_t ind = 0;
    VirtualTree::HuffmanNode *ptr_node = hft.root_node;

    for (; ind < excs.get_bit_size(); ind++)
    {
        excs >> bit;

        if (bit)
        {
            ptr_node->exception();
            ptr_node = ptr_node->get_right_node();
        }
        else
        {
            ptr_node = ptr_node->get_left_node();
        }
    }
}

BitString encode(BitString &data, Tree::HuffmanTree &hft)
{
    if (!data.get_size() && !data.get_bit_size())
    {
        throw std::runtime_error("no any data");
    }

    if (data.get_byte_padding() % 8)
    {
        throw std::runtime_error("data has padding");
    }

    BitString res, path;
    uint8_t loc_byte;

    std::vector<Tree::HuffmanKey> keys = hft.get_keys();

    if (!keys.size())
    {
        throw std::runtime_error("no any key");
    }

    data.reset_pos();

    for (uint64_t data_ind = 0; data_ind < data.get_size(); data_ind++)
    {
        bool is_find = false;

        data >> loc_byte;

        uint64_t key_ind = 0;

        for (; key_ind < keys.size(); key_ind++)
        {
            if (keys.at(key_ind).key.chr == loc_byte)
            {
                path = keys.at(key_ind).node->get_path();
                is_find = true;

                break;
            }
        }

        if (!is_find)
        {
            throw std::runtime_error("key not found");
        }

        res = res + path;
    }

    return res;
}

BitString decode(BitString &data, VirtualTree::HuffmanTree &hft)
{
    if (!data.get_bit_size())
    {
        throw std::runtime_error("no any data");
    }

    std::vector<VirtualTree::HuffmanKey> keys = hft.get_keys();

    if (hft.get_keys().size() < 2)
    {
        throw std::runtime_error("tree is incomplete");
    }

    data.reset_pos();

    bit loc_bit;
    bool can_read, padding;
    BitString res, chr;
    VirtualTree::HuffmanNode *ptr_node = hft.get_root_node();

    do
    {
        can_read = !(data >> loc_bit);
        padding = true;

        if (loc_bit)
        {
            ptr_node = ptr_node->get_right_node();
        }
        else
        {
            ptr_node = ptr_node->get_left_node();
        }

        if (ptr_node->has_value())
        {
            bool is_find = false;

            for (uint64_t ind = 0; ind < keys.size(); ind++)
            {
                if (keys.at(ind).node == ptr_node)
                {
                    load_into_bit_string(chr, keys.at(ind).key.chr, 0);
                    is_find = true;

                    break;
                }
            }

            if (!is_find)
            {
                throw std::runtime_error("key not found");
            }

            ptr_node = hft.get_root_node();
            padding = false;

            res = res + chr;
        }
    } while (can_read);

    if (padding)
    {
        throw std::runtime_error("data has padding");
    }

    return res;
}