/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/

#pragma once

#include "../StandardPluginsConfig.h"
#include <AzCore/std/containers/vector.h>
#include "../../../../EMStudioSDK/Source/DockWidgetPlugin.h"
#include <MysticQt/Source/DialogStack.h>
#include <MysticQt/Source/SearchButton.h>
#include <MysticQt/Source/ComboBox.h>
#include <EMotionFX/Source/MotionSet.h>
#include <EMotionFX/CommandSystem/Source/CommandManager.h>
#include <EMotionFX/CommandSystem/Source/MotionSetCommands.h>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QTreeWidget>
#include <QLineEdit>
#include <QDialog>


namespace EMStudio
{
    // forward declaration
    class MotionSetsWindowPlugin;

    class MotionSetRemoveMotionsFailedWindow
        : public QDialog
    {
        MCORE_MEMORYOBJECTCATEGORY(MotionSetRemoveMotionsFailedWindow, MCore::MCORE_DEFAULT_ALIGNMENT, MEMCATEGORY_STANDARDPLUGINS);

    public:
        MotionSetRemoveMotionsFailedWindow(QWidget* parent, const AZStd::vector<EMotionFX::Motion*>& motions);
    };


    class RenameMotionEntryWindow
        : public QDialog
    {
        Q_OBJECT
        MCORE_MEMORYOBJECTCATEGORY(RenameMotionEntryWindow, MCore::MCORE_DEFAULT_ALIGNMENT, MEMCATEGORY_STANDARDPLUGINS);

    public:
        RenameMotionEntryWindow(QWidget* parent, EMotionFX::MotionSet* motionSet, const AZStd::string& motionId);

    private slots:
        void TextEdited(const QString& text);
        void Accepted();

    private:
        EMotionFX::MotionSet*           mMotionSet;
        AZStd::vector<AZStd::string>    m_existingIds;
        AZStd::string                   m_motionId;
        QLineEdit*                      mLineEdit;
        QPushButton*                    mOKButton;
    };


    class MotionEditStringIDWindow
        : public QDialog
    {
        Q_OBJECT
        MCORE_MEMORYOBJECTCATEGORY(MotionEditStringIDWindow, MCore::MCORE_DEFAULT_ALIGNMENT, MEMCATEGORY_EMSTUDIOSDK)

    public:
        MotionEditStringIDWindow(QWidget* parent, EMotionFX::MotionSet* motionSet, const AZStd::vector<AZStd::string>& motionIDs);

    public slots:
        void Accepted();
        void StringABChanged(const QString& text);
        void CurrentIndexChanged(int index);

    private:
        void UpdateTableAndButton();

    private:
        EMotionFX::MotionSet*           mMotionSet;
        AZStd::vector<AZStd::string>    mMotionIDs;
        AZStd::vector<AZStd::string>    mModifiedMotionIDs;
        AZStd::vector<uint32>           mMotionToModifiedMap;
        AZStd::vector<uint32>           mValids;
        QTableWidget*                   mTableWidget;
        QLineEdit*                      mStringALineEdit;
        QLineEdit*                      mStringBLineEdit;
        QPushButton*                    mApplyButton;
        QLabel*                         mNumMotionIDsLabel;
        QLabel*                         mNumModifiedIDsLabel;
        QLabel*                         mNumDuplicateIDsLabel;
        MysticQt::ComboBox*             mComboBox;
    };


    class MotionSetTableWidget
        : public QTableWidget
    {
        Q_OBJECT
                 MCORE_MEMORYOBJECTCATEGORY(MotionSetTableWidget, MCore::MCORE_DEFAULT_ALIGNMENT, MEMCATEGORY_STANDARDPLUGINS);

    public:
        MotionSetTableWidget(MotionSetsWindowPlugin* parentPlugin, QWidget* parent);
        virtual ~MotionSetTableWidget();

    protected:
        // used for drag and drop support for the blend tree
        QMimeData* mimeData(const QList<QTableWidgetItem*> items) const override;
        QStringList mimeTypes() const override;
        Qt::DropActions supportedDropActions() const override;

        void dropEvent(QDropEvent* event) override;
        void dragEnterEvent(QDragEnterEvent* event) override;
        void dragMoveEvent(QDragMoveEvent* event) override;

        MotionSetsWindowPlugin* mPlugin;
    };


    class MotionSetWindow
        : public QWidget
    {
        Q_OBJECT
                 MCORE_MEMORYOBJECTCATEGORY(MotionSetWindow, MCore::MCORE_DEFAULT_ALIGNMENT, MEMCATEGORY_STANDARDPLUGINS);

    public:
        MotionSetWindow(MotionSetsWindowPlugin* parentPlugin, QWidget* parent);
        ~MotionSetWindow();

        bool Init();
        void ReInit();

        bool AddMotion(EMotionFX::MotionSet* motionSet, EMotionFX::MotionSet::MotionEntry* motionEntry);
        bool UpdateMotion(EMotionFX::MotionSet* motionSet, EMotionFX::MotionSet::MotionEntry* motionEntry, const AZStd::string& oldMotionId);
        bool RemoveMotion(EMotionFX::MotionSet* motionSet, EMotionFX::MotionSet::MotionEntry* motionEntry);

    public slots:
        void UpdateInterface();

        void OnAddNewEntry();
        void OnLoadEntries();
        void OnRemoveMotions();
        void RenameEntry(QTableWidgetItem* item);
        void OnRenameEntry();
        void OnUnassignMotions();
        void OnCopyMotionID();
        void OnClearMotions();
        void OnEditButton();

        void SearchStringChanged(const QString& text);

        void OnEntryDoubleClicked(QTableWidgetItem* item);

        void dropEvent(QDropEvent* event) override;
        void dragEnterEvent(QDragEnterEvent* event) override;

    private:
        virtual void keyPressEvent(QKeyEvent* event) override;
        virtual void keyReleaseEvent(QKeyEvent* event) override;

        bool InsertRow(EMotionFX::MotionSet* motionSet, EMotionFX::MotionSet::MotionEntry* motionEntry, QTableWidget* tableWidget, bool readOnly);
        bool FillRow(EMotionFX::MotionSet* motionSet, EMotionFX::MotionSet::MotionEntry* motionEntry, uint32 rowIndex, QTableWidget* tableWidget, bool readOnly);
        bool RemoveRow(EMotionFX::MotionSet* motionSet, EMotionFX::MotionSet::MotionEntry* motionEntry, QTableWidget* tableWidget);

        void UpdateMotionSetTable(QTableWidget* tableWidget, EMotionFX::MotionSet* motionSet, bool readOnly = false);
        void AddMotions(const AZStd::vector<AZStd::string>& filenames);
        void contextMenuEvent(QContextMenuEvent* event) override;

        EMotionFX::MotionSet::MotionEntry* FindMotionEntry(QTableWidgetItem* item) const;

        void GetRowIndices(const QList<QTableWidgetItem*>& items, AZStd::vector<int>& outRowIndices);
        uint32 CalcNumMotionEntriesUsingMotionExcluding(const AZStd::string& motionFilename, EMotionFX::MotionSet* excludedMotionSet);

    private:
        QVBoxLayout*                            mVLayout;
        MotionSetTableWidget*                   m_tableWidget;

        QPushButton*                            mAddButton;
        QPushButton*                            mLoadButton;
        QPushButton*                            mRemoveButton;
        QPushButton*                            mClearButton;
        QPushButton*                            mEditButton;

        MysticQt::SearchButton*                 mFindWidget;
        MotionSetsWindowPlugin*                 mPlugin;
    };
} // namespace EMStudio