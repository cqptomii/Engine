#include "engine/serialization/document/blockview.hpp"

/**
 * @brief Method used to get the root block of the document
 * 
 * @return std::optional<BlockView> : the root block of the document
 */
 std::optional<BlockView> Document::root() const{
    //Check if the document is empty
    if(this->blocks_.empty())
        return std::nullopt;

    //Return the root block
    return BlockView(this, 0);
}