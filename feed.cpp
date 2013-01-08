#include "feed.h"

Feed::Feed()
{
}

void Feed::addItem(Item* item)
{
    m_items.append(item);
}


