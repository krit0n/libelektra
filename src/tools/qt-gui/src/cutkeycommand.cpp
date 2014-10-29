#include "cutkeycommand.hpp"

CutKeyCommand::CutKeyCommand(QString type, TreeViewModel *sourceParentModel, ConfigNode *source, ConfigNode *target, int index, QUndoCommand *parent)
    : QUndoCommand(parent)
    , m_sourceParentModel(sourceParentModel)
    , m_source(new ConfigNode(*source))
    , m_target(target)
    , m_isExpanded(false)
    , m_sourceIndex(index)
    , m_targetIndex(-1)
{
    setText(type);

    QString newPath = m_target->getPath() + "/" + m_source->getName();
    m_source->setPath(newPath);
}

void CutKeyCommand::undo()
{
    m_isExpanded = m_target->getIsExpanded();
    m_sourceParentModel->insertRow(m_sourceIndex, m_source);
    m_target->getChildren()->removeRow(m_targetIndex);

    if(m_sourceParentModel == m_target->getChildren()){
        m_sourceParentModel->refresh();
    }
}

void CutKeyCommand::redo()
{
    m_target->setIsExpanded(m_isExpanded);
    m_target->appendChild(m_source);
    m_sourceParentModel->removeRow(m_sourceIndex);

    if(m_sourceParentModel == m_target->getChildren()){
        m_targetIndex = m_target->getChildCount();
        m_sourceParentModel->refresh();
    }
    else
        m_targetIndex = m_target->getChildCount() - 1;
}
