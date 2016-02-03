#include "mecabnode.h"

static const QString MeCabUnknownFeature = "Unknown";

MeCabNode::MeCabNode(const QString &surface, const QString &feature, int cost) :
    surface_(surface),
    feature_(feature),
    features_(feature.split(",")),
    cost_(cost)
{

}

MeCabNode::~MeCabNode()
{

}

bool MeCabNode::isUnknown() const
{
    return this->feature_ == MeCabUnknownFeature;
}

MeCabNode MeCabNode::create(const MeCab::Node *node)
{
    QString surface = QString::fromUtf8(node->surface, node->length);
    QString feature = QString::fromUtf8(node->feature);

    return MeCabNode(surface, feature, node->cost);
}

MeCabNode MeCabNode::create_dummy(const QString &surface, const QString &feature, int cost)
{
    return MeCabNode(surface, feature, cost);
}

MeCabResult MeCabNode::create_nodes(const MeCab::Node *node)
{
    MeCabResult result;
    while ( node ) {
        MeCabNode item = MeCabNode::create(node);
        if ( ! item.feature().startsWith("BOS/EOS") ) {
            result.append(item);
        }
        node = node->next;
    }
    return result;
}

QString MeCabNode::get_feature(int index) const
{
    if ( this->features_.size() <= index ) {
        return QString();
    }
    return this->features_[index];
}
