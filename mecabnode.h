#ifndef MECABNODE_H
#define MECABNODE_H

#include <QStringList>

#include <mecab.h>

const QString MeCabUnkownFeature;

class MeCabNode
{
public:
    typedef QList<MeCabNode> MeCabResult;

    ~MeCabNode();

    bool isUnknown() const;

    QString surface() const
    { return this->surface_; }
    QString feature() const
    { return this->feature_; }

    int cost() const
    { return this->cost_; }

    // features
    QString parts() const
    { return this->get_feature(0); }
    QString parts_detail1() const
    { return this->get_feature(1); }
    QString parts_detail2() const
    { return this->get_feature(2); }
    QString parts_detail3() const
    { return this->get_feature(3); }
    QStringList parts_details() const
    { return {this->parts_detail1(), this->parts_detail2(), this->parts_detail3()}; }
    QString conjugated_form() const
    { return this->get_feature(4); }
    QString conjugated_type() const
    { return this->get_feature(5); }
    QString prototype() const
    { return this->get_feature(6); }
    QString kana() const
    { return this->get_feature(7); }
    QString speech() const
    { return this->get_feature(8); }

    bool hasKana() const
    { return ! this->kana().isEmpty(); }
    bool hasSpeech() const
    { return ! this->speech().isEmpty(); }

    static MeCabNode create(const MeCab::Node *node);
    static MeCabResult create_nodes(const MeCab::Node *node);

private:
    MeCabNode(const QString &surface, const QString &feature, int cost);

    QString get_feature(int index) const;

    QString surface_;
    QString feature_;
    QStringList features_;
    int cost_;

};

typedef MeCabNode::MeCabResult MeCabResult;

#endif // MECABNODE_H
