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
// Original file Copyright Crytek GMBH or its affiliates, used under license.

#ifndef CRYINCLUDE_EDITOR_TRACKVIEW_TRACKVIEWSEQUENCE_H
#define CRYINCLUDE_EDITOR_TRACKVIEW_TRACKVIEWSEQUENCE_H
#pragma once

#include "IMovieSystem.h"

#include <AzToolsFramework/UI/PropertyEditor/PropertyEditorAPI.h>

#include "TrackViewAnimNode.h"
#include "Objects/SequenceObject.h"
#include "Undo/Undo.h"

struct ITrackViewSequenceListener
{
    // Called when sequence settings (time range, flags) have changed
    virtual void OnSequenceSettingsChanged(CTrackViewSequence* pSequence) {}

    enum ENodeChangeType
    {
        eNodeChangeType_Added,
        eNodeChangeType_Removed,
        eNodeChangeType_Expanded,
        eNodeChangeType_Collapsed,
        eNodeChangeType_Hidden,
        eNodeChangeType_Unhidden,
        eNodeChangeType_Enabled,
        eNodeChangeType_Disabled,
        eNodeChangeType_Muted,
        eNodeChangeType_Unmuted,
        eNodeChangeType_Selected,
        eNodeChangeType_Deselected,
        eNodeChangeType_SetAsActiveDirector,
        eNodeChangeType_NodeOwnerChanged
    };

    // Called when a node is changed
    virtual void OnNodeChanged(CTrackViewNode* pNode, ENodeChangeType type) {}

    // Called when a node is added
    virtual void OnNodeRenamed(CTrackViewNode* pNode, const char* pOldName) {}

    // Called when selection of nodes changed
    virtual void OnNodeSelectionChanged(CTrackViewSequence* pSequence) {}

    // Called when selection of keys changed.
    virtual void OnKeySelectionChanged(CTrackViewSequence* pSequence) {}

    // Called when keys in a track changed
    virtual void OnKeysChanged(CTrackViewSequence* pSequence) {}

    // Called when a new key is added to a track
    virtual void OnKeyAdded(CTrackViewKeyHandle& addedKeyHandle) {}
};

struct ITrackViewSequenceManagerListener
{
    virtual void OnSequenceAdded(CTrackViewSequence* pSequence) {}
    virtual void OnSequenceRemoved(CTrackViewSequence* pSequence) {}
    virtual void OnLegacySequencePostLoad(CTrackViewSequence* sequence, bool undo) {}
};

////////////////////////////////////////////////////////////////////////////
//
// This class represents a IAnimSequence in TrackView and contains
// the editor side code for changing it
//
////////////////////////////////////////////////////////////////////////////
class CTrackViewSequence
    : public CTrackViewAnimNode
    , public IUndoManagerListener
    , public ITrackViewSequence
    , public ITrackViewSequenceManagerListener
    , public AzToolsFramework::PropertyEditorEntityChangeNotificationBus::MultiHandler
{
    friend class CTrackViewSequenceManager;
    friend class CTrackViewNode;
    friend class CTrackViewAnimNode;
    friend class CTrackViewTrack;
    friend class CTrackViewSequenceNotificationContext;
    friend class CTrackViewSequenceNoNotificationContext;

    // Undo friends
    friend class CAbstractUndoTrackTransaction;
    friend class CAbstractUndoAnimNodeTransaction;
    friend class CUndoAnimNodeReparent;
    friend class CUndoTrackObject;
    friend class CAbstractUndoSequenceTransaction;

public:
    CTrackViewSequence(IAnimSequence* pSequence);
    CTrackViewSequence(AZStd::intrusive_ptr<IAnimSequence>& sequence);
    ~CTrackViewSequence();

    // Called after de-serialization of IAnimSequence
    void Load() override;

    // ITrackViewNode
    virtual ETrackViewNodeType GetNodeType() const override { return eTVNT_Sequence; }

    virtual const char* GetName() const override { return m_pAnimSequence->GetName(); }
    virtual bool SetName(const char* pName) override;
    virtual bool CanBeRenamed() const override { return true; }

    // Binding/Unbinding
    virtual void BindToEditorObjects() override;
    virtual void UnBindFromEditorObjects() override;
    virtual bool IsBoundToEditorObjects() const override;

    // Time range
    void SetTimeRange(Range timeRange);
    Range GetTimeRange() const;

    // Current time in sequence. Note that this can be different from the time
    // of the animation context, if this sequence is used as a sub sequence
    const float GetTime() const;

    // CryMovie Flags
    void SetFlags(IAnimSequence::EAnimSequenceFlags flags);
    IAnimSequence::EAnimSequenceFlags GetFlags() const;

    // Get sequence object in scene
    CSequenceObject* GetSequenceObject() const { return static_cast<CSequenceObject*>(m_pAnimSequence->GetLegacySequenceObject()); }
    AZ::EntityId     GetSequenceComponentEntityId() const { return m_pAnimSequence.get() ? m_pAnimSequence->GetSequenceEntityId() : AZ::EntityId(); }

    // Get the Object Layer that the sequence entity is in
    CObjectLayer*    GetSequenceObjectLayer() const;

    // Check if this node belongs to a sequence
    bool IsAncestorOf(CTrackViewSequence* pSequence) const;

    // Get single selected key if only one key is selected
    CTrackViewKeyHandle FindSingleSelectedKey();

    // Get CryMovie sequence ID
    uint32 GetCryMovieId() const { return m_pAnimSequence->GetId(); }

    // Rendering
    virtual void Render(const SAnimContext& animContext) override;

    // Playback control
    virtual void Animate(const SAnimContext& animContext) override;
    void Resume() { m_pAnimSequence->Resume(); }
    void Pause() { m_pAnimSequence->Pause(); }
    void StillUpdate() { m_pAnimSequence->StillUpdate(); }

    void OnLoop() { m_pAnimSequence->OnLoop(); }

    // Active & deactivate
    void Activate() { m_pAnimSequence->Activate(); }
    void Deactivate() { m_pAnimSequence->Deactivate(); }
    void PrecacheData(const float time) { m_pAnimSequence->PrecacheData(time); }

    // Begin & end cut scene
    void BeginCutScene(const bool bResetFx) const;
    void EndCutScene() const;

    // Reset
    void Reset(const bool bSeekToStart) { m_pAnimSequence->Reset(bSeekToStart); }
    void ResetHard() { m_pAnimSequence->ResetHard(); }

    void TimeChanged(float newTime) { m_pAnimSequence->TimeChanged(newTime); }

    // Check if the sequence's layer is locked
    bool IsLayerLocked() const;

    // Check if it's a group node
    virtual bool IsGroupNode() const override { return true; }

    // Track Events (TODO: Undo?)
    int GetTrackEventsCount() const      { return m_pAnimSequence->GetTrackEventsCount(); }
    const char* GetTrackEvent(int index) { return m_pAnimSequence->GetTrackEvent(index); }
    bool AddTrackEvent(const char* szEvent)                            { MarkAsModified(); return m_pAnimSequence->AddTrackEvent(szEvent); }
    bool RemoveTrackEvent(const char* szEvent)                         { MarkAsModified(); return m_pAnimSequence->RemoveTrackEvent(szEvent); }
    bool RenameTrackEvent(const char* szEvent, const char* szNewEvent) { MarkAsModified(); return m_pAnimSequence->RenameTrackEvent(szEvent, szNewEvent); }
    bool MoveUpTrackEvent(const char* szEvent)                         { MarkAsModified(); return m_pAnimSequence->MoveUpTrackEvent(szEvent); }
    bool MoveDownTrackEvent(const char* szEvent)                       { MarkAsModified(); return m_pAnimSequence->MoveDownTrackEvent(szEvent); }
    void ClearTrackEvents()                                            { MarkAsModified(); m_pAnimSequence->ClearTrackEvents(); }

    // Deletes all selected nodes (re-parents childs if group node gets deleted)
    void DeleteSelectedNodes();

    // Select selected nodes in viewport
    void SelectSelectedNodesInViewport();

    // Deletes all selected keys
    void DeleteSelectedKeys();

    // Sync from/to base
    void SyncSelectedTracksToBase();
    void SyncSelectedTracksFromBase();

    // Listeners
    void AddListener(ITrackViewSequenceListener* pListener);
    void RemoveListener(ITrackViewSequenceListener* pListener);

    // Checks if this is the active sequence in TV
    bool IsActiveSequence() const;

    // The root sequence node is always an active director
    virtual bool IsActiveDirector() const override { return true; }

    // Stores track undo objects for tracks with selected keys
    void StoreUndoForTracksWithSelectedKeys();

    // Copy keys to clipboard (in XML form)
    void CopyKeysToClipboard(const bool bOnlySelectedKeys, const bool bOnlyFromSelectedTracks);

    // Paste keys from clipboard. Tries to match the given data to the target track first,
    // then the target anim node and finally the whole sequence. If it doesn't find any
    // matching location, nothing will be pasted. Before pasting the given time offset is
    // applied to the keys.
    void PasteKeysFromClipboard(CTrackViewAnimNode* pTargetNode, CTrackViewTrack* pTargetTrack, const float timeOffset = 0.0f);

    // Returns a vector of pairs that match the XML track nodes in the clipboard to the tracks in the sequence for pasting.
    // It is used by PasteKeysFromClipboard directly and to preview the locations of the to be pasted keys.
    typedef std::pair<CTrackViewTrack*, XmlNodeRef> TMatchedTrackLocation;
    std::vector<TMatchedTrackLocation> GetMatchedPasteLocations(XmlNodeRef clipboardContent, CTrackViewAnimNode* pTargetNode, CTrackViewTrack* pTargetTrack);

    // Adjust the time range
    void AdjustKeysToTimeRange(Range newTimeRange);

    // Clear all key selection
    void DeselectAllKeys();

    // Offset all key selection
    void OffsetSelectedKeys(const float timeOffset);
    // Scale all selected keys by this offset.
    void ScaleSelectedKeys(const float timeOffset);
    //! Push all the keys which come after the first key in time among selected ones by this offset.
    void SlideKeys(const float timeOffset);
    //! Clone all selected keys
    void CloneSelectedKeys();

    // Limit the time offset so as to keep all involved keys in range when offsetting.
    float ClipTimeOffsetForOffsetting(const float timeOffset);
    // Limit the time offset so as to keep all involved keys in range when scaling.
    float ClipTimeOffsetForScaling(const float timeOffset);
    // Limit the time offset so as to keep all involved keys in range when sliding.
    float ClipTimeOffsetForSliding(const float timeOffset);

    // Notifications
    void OnSequenceSettingsChanged();
    void OnKeySelectionChanged();
    void OnKeysChanged();
    void OnKeyAdded(CTrackViewKeyHandle& addedKeyHandle);
    void OnNodeSelectionChanged();
    void OnNodeChanged(CTrackViewNode* pNode, ITrackViewSequenceListener::ENodeChangeType type);
    void OnNodeRenamed(CTrackViewNode* pNode, const char* pOldName);

    // IAnimNodeOwner
    void MarkAsModified() override;
    // ~IAnimNodeOwner

    SequenceType GetSequenceType() const 
    {
        if (m_pAnimSequence.get())
        {
            return m_pAnimSequence->GetSequenceType();
        }
        else
        {
            return kSequenceTypeDefault;
        }
    }

    // Called when the 'Record' button is pressed in the toolbar
    void SetRecording(bool enableRecording);

    /////////////////////////////////////////////////////////////////////////////////////////////////////
    // PropertyEditorEntityChangeNotificationBus handler
    void OnEntityComponentPropertyChanged(AZ::ComponentId /*changedComponentId*/) override;
    /////////////////////////////////////////////////////////////////////////////////////////////////////

private:
    // These are used to avoid listener notification spam via CTrackViewSequenceNotificationContext.
    // For recursion there is a counter that increases on QueueListenerNotifications
    // and decreases on SubmitPendingListenerNotifcations
    // Only when the counter reaches 0 again SubmitPendingListenerNotifcations
    // will submit the notifications
    void QueueNotifications();
    void SubmitPendingNotifcations(bool force = false);

    /////////////////////////////////////////////////////////////////////////
    // overrides for ITrackViewSequenceManagerListener
    void OnSequenceRemoved(CTrackViewSequence* pSequence) override;
    void OnSequenceAdded(CTrackViewSequence* pSequence) override;

    // Called when an animation updates needs to be schedules
    void ForceAnimation();

    virtual void CopyKeysToClipboard(XmlNodeRef& xmlNode, const bool bOnlySelectedKeys, const bool bOnlyFromSelectedTracks) override;

    void UpdateLightAnimationRefs(const char* pOldName, const char* pNewName);

    std::deque<CTrackViewTrack*> GetMatchingTracks(CTrackViewAnimNode* pAnimNode, XmlNodeRef trackNode);
    void GetMatchedPasteLocationsRec(std::vector<TMatchedTrackLocation>& locations, CTrackViewNode* pCurrentNode, XmlNodeRef clipboardNode);

    virtual void BeginUndoTransaction();
    virtual void EndUndoTransaction();
    virtual void BeginRestoreTransaction();
    virtual void EndRestoreTransaction();

    // For record mode on AZ::Entities - connect (or disconnect) to buses for notification of property changes
    void ConnectToBusesForRecording(const AZ::EntityId& entityIdForBus, bool enableConnection);

    // Searches for current property vs. Track values for the given node and sets a key for all values that differ.
    // Returns the number of keys set
    int RecordTrackChangesForNode(CTrackViewAnimNode* componentNode);

    // Current time when animated
    float m_time;

    // Stores if sequence is bound
    bool m_bBoundToEditorObjects = false;

    AZStd::intrusive_ptr<IAnimSequence> m_pAnimSequence;
    std::vector<ITrackViewSequenceListener*> m_sequenceListeners;

    // Notification queuing
    unsigned int m_selectionRecursionLevel = 0;
    bool m_bNoNotifications = false;
    bool m_bQueueNotifications = false;
    bool m_bNodeSelectionChanged = false;
    bool m_bForceAnimation = false;
    bool m_bKeySelectionChanged = false;
    bool m_bKeysChanged = false;
};

////////////////////////////////////////////////////////////////////////////
class CTrackViewSequenceNotificationContext
{
public:
    CTrackViewSequenceNotificationContext(CTrackViewSequence* pSequence)
        : m_pSequence(pSequence)
    {
        if (m_pSequence)
        {
            m_pSequence->QueueNotifications();
        }
    }

    ~CTrackViewSequenceNotificationContext()
    {
        if (m_pSequence)
        {
            m_pSequence->SubmitPendingNotifcations();
        }
    }

private:
    CTrackViewSequence* m_pSequence;
};

////////////////////////////////////////////////////////////////////////////
class CTrackViewSequenceNoNotificationContext
{
public:
    CTrackViewSequenceNoNotificationContext(CTrackViewSequence* pSequence)
        : m_pSequence(pSequence)
        , m_bNoNotificationsPreviously(false)
    {
        if (m_pSequence)
        {
            m_bNoNotificationsPreviously = m_pSequence->m_bNoNotifications;
            m_pSequence->m_bNoNotifications = true;
        }
    }

    ~CTrackViewSequenceNoNotificationContext()
    {
        if (m_pSequence)
        {
            m_pSequence->m_bNoNotifications = m_bNoNotificationsPreviously;
        }
    }

private:
    CTrackViewSequence* m_pSequence;

    // Reentrance could happen if there are overlapping sub-sequences controlling
    // the same camera.
    bool m_bNoNotificationsPreviously;
};
#endif // CRYINCLUDE_EDITOR_TRACKVIEW_TRACKVIEWSEQUENCE_H
