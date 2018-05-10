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

#ifndef __EMSTUDIO_PREFERENCESWINDOW_H
#define __EMSTUDIO_PREFERENCESWINDOW_H

// include required headers
#include "EMStudioConfig.h"
#include <MCore/Source/StandardHeaders.h>
#include <MysticQt/Source/PropertyWidget.h>
#include <QDialog>


// forward declarations
QT_FORWARD_DECLARE_CLASS(QStackedWidget)
QT_FORWARD_DECLARE_CLASS(QListWidget)
QT_FORWARD_DECLARE_CLASS(QListWidgetItem)

namespace EMStudio
{
    // forward declaration
    class EMStudioPlugin;

    class EMSTUDIO_API PreferencesWindow
        : public QDialog
    {
        Q_OBJECT
        MCORE_MEMORYOBJECTCATEGORY(PreferencesWindow, MCore::MCORE_DEFAULT_ALIGNMENT, MEMCATEGORY_EMSTUDIOSDK)

    public:
        struct Category
        {
            MCORE_MEMORYOBJECTCATEGORY(PreferencesWindow::Category, MCore::MCORE_DEFAULT_ALIGNMENT, MEMCATEGORY_EMSTUDIOSDK)

            QWidget *                    mWidget;
            MysticQt::PropertyWidget*   mPropertyWidget;
            QListWidgetItem*            mListWidgetItem;
            AZStd::string               mName;
        };

        PreferencesWindow(QWidget* parent);
        virtual ~PreferencesWindow();

        void Init();
        void AddCategoriesFromPlugin(EMStudioPlugin* plugin);
        MysticQt::PropertyWidget* AddCategory(const char* categoryName, const char* relativeFileName, bool readOnly);
        void AddCategory(QWidget* widget, const char* categoryName, const char* relativeFileName, bool readOnly);

        MCORE_INLINE Category* GetCategory(uint32 index)                                                    { return mCategories[index]; }
        MCORE_INLINE uint32 GetNumCategories()                                                              { return mCategories.GetLength(); }
        Category* FindCategoryByName(const char* categoryName) const;
        MCORE_INLINE MysticQt::PropertyWidget* FindPropertyWidgetByName(const char* categoryName) const
        {
            Category* category = FindCategoryByName(categoryName);
            if (category == nullptr)
            {
                return nullptr;
            }
            else
            {
                return category->mPropertyWidget;
            }
        }

    public slots:
        void ChangePage(QListWidgetItem* current, QListWidgetItem* previous);

    private:
        MCore::Array<Category*>                     mCategories;
        QStackedWidget*                             mStackedWidget;
        QListWidget*                                mCategoriesWidget;
    };
} // namespace EMStudio


#endif
