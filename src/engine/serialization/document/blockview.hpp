/**
 * @file blockview.hpp
 * @author Tom FRAISSE
 * @brief Block view class used as a interface to access the information about a block in the document
 * @version 0.1
 * @date 2026-07-31
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#ifndef BLOCKVIEW_HPP
#define BLOCKVIEW_HPP

#include "engine/serialization/document/document.hpp"
#include <cstdint>
#include <string_view>
#include <glm/glm.hpp>

class BlockView {
private:

    // The document associated with the block view
    const Document* doc_{nullptr};

    // The index of the block in the document
    std::uint32_t block_index_{0};

public:
    /**
     * @brief Default constructor used to initialize the block view
     * 
     */
    BlockView() = default;

    /**
     * @brief Constructor used to initialize the block view with a document and a block index
     * 
     * @param doc : The document associated with the block view
     * @param block_index : The index of the block in the document
     */
    BlockView(const Document* doc, std::uint32_t block_index) noexcept : doc_(doc), block_index_(block_index) {}

    /**
     * @brief Method used to check if the block view is valid
     * 
     * @return true : If the block view is valid
     * @return false : If the block view is not valid
     */
    bool valid() const noexcept{
        return doc_ != nullptr && block_index_ < doc_->blocks().size();
    }


    /**
     * @brief Method used to get the name of the block
     * 
     * @return std::string_view : The name of the block
     */
    std::string_view name() const{
        if(!valid()) return std::string_view();
        return doc_->blocks()[block_index_].name;
    }
    /**
     * @brief Method used to get the line number of the block
     * 
     * @return std::uint32_t : The line number of the block
     */
    std::uint32_t line() const{
        // Check if the block is valid
        if(!valid()) return 0;
        return doc_->blocks()[block_index_].line;
    }
};

#endif