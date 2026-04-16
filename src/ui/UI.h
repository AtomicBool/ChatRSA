#pragma once

#include <vector>
#include <string>
#include "imgui.h"
#include "utils/Contact.h"

//
// =========================
// UI EVENTS (UI -> App)
// =========================
//

struct UIEvent
{
    enum class Type
    {
        None,
        AddContact,
        SelectContact
    } type = Type::None;

    // 只在对应 type 下有效
    struct AddContactPayload
    {
        std::string name;
        std::string publicKey;
    } addContact{};

    struct SelectContactPayload
    {
        Contact selected;
    } selectContact{};
};

//
// =========================
// VIEW MODEL (App -> UI)
// =========================
//

struct ContactViewModel
{
    std::vector<Contact> contacts;
};

//
// =========================
// UI STATE (pure UI state)
// =========================
//
struct UIState
{
    // public
        // flags
        bool debug = false;
        bool display = false;
        bool firstFrame = false;

        // events
        std::vector<UIEvent> events;

        // input
        char searchBuffer[256] = { 0 };

    // private
        // layout
        float sizesPercentage[2] = { 0.45f, 0.6f };

        // add contact
        bool showAddContact = false;
        char addName[128] = { 0 };
        char addKey[512] = { 0 };

        // selection
        std::string selectedContactName = "";

    // -------------------------
    // event helpers
    // -------------------------
    // public
        void ClearEvents()
        {
            events.clear();
        }
    // private
        void PushAddContact(const std::string& name, const std::string& key)
        {
            UIEvent e;
            e.type = UIEvent::Type::AddContact;
            e.addContact = { name, key };
            events.push_back(e);
        }

        void PushSelectContact(Contact m_contact)
        {
            UIEvent e;
            e.type = UIEvent::Type::SelectContact;
            e.selectContact = { m_contact };
            events.push_back(e);
        }
};

//
// =========================
// UI RENDER API
// =========================
//

namespace UI
{
    void Render(UIState& state, const ContactViewModel& vm);
}