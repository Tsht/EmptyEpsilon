#include "playerInfo.h"
#include "spaceObjects/playerSpaceship.h"
#include "commsOverlay.h"
#include "gui/gui2_panel.h"
#include "gui/gui2_progressbar.h"
#include "gui/gui2_button.h"
#include "gui/gui2_label.h"
#include "gui/gui2_scrolltext.h"
#include "gui/gui2_listbox.h"
#include "gui/gui2_textentry.h"
#include "gui/gui2_element.h"
#include "gui/gui2_togglebutton.h"

#include "onScreenKeyboard.h"

GuiCommsOverlay::GuiCommsOverlay(GuiContainer* owner)
: GuiElement(owner, "COMMS_OVERLAY")
{
    // Panel for reporting outgoing hails.
    opening_box = new GuiPanel(this, "COMMS_OPENING_BOX");
    opening_box->hide()->setSize(800, 100)->setPosition(0, -250, sp::Alignment::BottomCenter);
    (new GuiLabel(opening_box, "COMMS_OPENING_LABEL", tr("Opening communications..."), 40))->setSize(GuiElement::GuiSizeMax, 50)->setPosition(0, 0, sp::Alignment::TopCenter);
    opening_progress = new GuiProgressbar(opening_box, "COMMS_OPENING_PROGRESS", PlayerSpaceship::comms_channel_open_time, 0.0, 0.0);
    opening_progress->setSize(500, 40)->setPosition(50, -10, sp::Alignment::BottomLeft);

    // Cancel button closes the communication.
    opening_cancel = new GuiButton(opening_box, "COMMS_OPENING_CANCEL", tr("button", "Cancel"), []()
    {
        if (my_spaceship)
            my_spaceship->commandCloseTextComm();
    });
    opening_cancel->setSize(200, 40)->setPosition(-50, -10, sp::Alignment::BottomRight);

    // Panel for reporting incoming hails.
    hailed_box = new GuiPanel(this, "COMMS_BEING_HAILED_BOX");
    hailed_box->hide()->setSize(800, 140)->setPosition(0, -250, sp::Alignment::BottomCenter);
    hailed_label = new GuiLabel(hailed_box, "COMMS_BEING_HAILED_LABEL", "..", 40);
    hailed_label->setSize(GuiElement::GuiSizeMax, 50)->setPosition(0, 20, sp::Alignment::TopCenter);

    // Buttons to answer or ignore hails.
    hailed_answer = new GuiButton(hailed_box, "COMMS_BEING_HAILED_ANSWER", tr("Answer"), []() {
        if (my_spaceship)
            my_spaceship->commandAnswerCommHail(true);
    });
    hailed_answer->setSize(300, 50)->setPosition(20, -20, sp::Alignment::BottomLeft);

    hailed_ignore = new GuiButton(hailed_box, "COMMS_BEING_HAILED_IGNORE", tr("Ignore"), []() {
        if (my_spaceship)
            my_spaceship->commandAnswerCommHail(false);
    });
    hailed_ignore->setSize(300, 50)->setPosition(-20, -20, sp::Alignment::BottomRight);

    // Panel for unresponsive hails.
    no_response_box = new GuiPanel(this, "COMMS_OPENING_BOX");
    no_response_box->hide()->setSize(800, 70)->setPosition(0, -250, sp::Alignment::BottomCenter);
    (new GuiLabel(no_response_box, "COMMS_NO_REPONSE_LABEL", tr("No reply..."), 40))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setPosition(0, 0, sp::Alignment::TopLeft);

    // Button to acknowledge unresponsive hails.
    (new GuiButton(no_response_box, "COMMS_NO_REPLY_OK", "Ok", []() {
        if (my_spaceship)
            my_spaceship->commandCloseTextComm();
    }))->setSize(100, 50)->setPosition(-20, -10, sp::Alignment::BottomRight);

    // Panel for broken communications.
    broken_box = new GuiPanel(this, "COMMS_BROKEN_BOX");
    broken_box->hide()->setSize(800, 70)->setPosition(0, -250, sp::Alignment::BottomCenter);
    (new GuiLabel(broken_box, "COMMS_BROKEN_LABEL", tr("Communications were suddenly cut"), 40))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setPosition(0, 0, sp::Alignment::TopLeft);

    // Button to acknowledge broken communications.
    (new GuiButton(broken_box, "COMMS_BROKEN_OK", "Ok", []() {
        if (my_spaceship)
            my_spaceship->commandCloseTextComm();
    }))->setSize(100, 50)->setPosition(-20, -10, sp::Alignment::BottomRight);

    // Panel for communications closed by the other object.
    closed_box = new GuiPanel(this, "COMMS_CLOSED_BOX");
    closed_box->hide()->setSize(800, 70)->setPosition(0, -250, sp::Alignment::BottomCenter);
    (new GuiLabel(closed_box, "COMMS_BROKEN_LABEL", tr("Communications channel closed"), 40))->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax)->setPosition(0, 0, sp::Alignment::TopLeft);

    // Button to acknowledge closed communications.
    (new GuiButton(closed_box, "COMMS_CLOSED_OK", "Ok", []() {
        if (my_spaceship)
            my_spaceship->commandCloseTextComm();
    }))->setSize(100, 50)->setPosition(-20, -10, sp::Alignment::BottomRight);


    // Panel for chat communications with GMs and other player ships.
     // Panel de titre
    chat_comms_title = new GuiPanel(this, "HACK_TITLE_BOX");
    chat_comms_title ->setSize(600, 50)->setPosition(0, -720, sp::Alignment::BottomCenter)->hide();

    chat_label = new GuiLabel(chat_comms_title, "", "", 30);
    chat_label->setSize(GuiElement::GuiSizeMax, GuiElement::GuiSizeMax);

        // Button to close chat comms.
    chat_comms_close_button = new GuiButton(chat_comms_title, "CLOSE_BUTTON", tr("button", "Close"), [this]() {
        if (my_spaceship)
            my_spaceship->commandCloseTextComm();
    });
    chat_comms_close_button->setTextSize(20)->setPosition(-10, 10, sp::Alignment::TopRight)->setSize(60, 30);

    chat_minimize_button = new GuiToggleButton(chat_comms_title, "", "-", [this](bool value)
    {
        minimize(value);
    });
    chat_minimize_button->setPosition(-70, 10, sp::Alignment::TopRight)->setSize(60, 30);

    minimized = false;

    chat_comms_box = new GuiPanel(this, "COMMS_CHAT_BOX");
    chat_comms_box->hide()->setSize(800, 600)->setPosition(0, -100, sp::Alignment::BottomCenter);

    // Message entry field for chat.
    chat_comms_message_entry = new GuiTextEntry(chat_comms_box, "COMMS_CHAT_MESSAGE_ENTRY", "");
    chat_comms_message_entry->setPosition(20, -20, sp::Alignment::BottomLeft)->setSize(640, 50);
    chat_comms_message_entry->enterCallback([this](string text){
        if (my_spaceship)
            my_spaceship->commandSendCommPlayer(chat_comms_message_entry->getText());
        chat_comms_message_entry->setText("");
    });

    // Text of incoming chat messages.
    chat_comms_text = new GuiScrollText(chat_comms_box, "COMMS_CHAT_TEXT", "");
    chat_comms_text->enableAutoScrollDown()->setPosition(20, 30, sp::Alignment::TopLeft)->setSize(760, 500);

    // Button to send a message.
    chat_comms_send_button = new GuiButton(chat_comms_box, "SEND_BUTTON", tr("button", "Send"), [this]() {
        if (my_spaceship)
            my_spaceship->commandSendCommPlayer(chat_comms_message_entry->getText());
        chat_comms_message_entry->setText("");
    });
    chat_comms_send_button->setPosition(-20, -20, sp::Alignment::BottomRight)->setSize(120, 50);

    // Button to close chat comms.
    chat_comms_close_button = new GuiButton(chat_comms_box, "CLOSE_BUTTON", tr("button", "Close"), []() {
        if (my_spaceship)
            my_spaceship->commandCloseTextComm();
    });
    chat_comms_close_button->setTextSize(20)->setPosition(-10, 0, sp::Alignment::TopRight)->setSize(70, 30);

    if (!engine->getObject("mouseRenderer")) //If we are a touch screen, add a on screen keyboard.
    {
        OnScreenKeyboardControl* keyboard = new OnScreenKeyboardControl(chat_comms_box, chat_comms_message_entry);
        keyboard->setPosition(20, -20, sp::Alignment::BottomLeft)->setSize(760, 200);
        chat_comms_message_entry->setPosition(20, -220, sp::Alignment::BottomLeft);
        chat_comms_send_button->setPosition(-20, -220, sp::Alignment::BottomRight);
        chat_comms_text->setSize(chat_comms_text->getSize().x, chat_comms_text->getSize().y - 200);
    }

    // Panel for scripted comms with objects.
    script_comms_box = new GuiPanel(this, "COMMS_SCRIPT_BOX");
    script_comms_box->hide()->setSize(800, 600)->setPosition(0, -100, sp::Alignment::BottomCenter);

    script_comms_text = new GuiScrollText(script_comms_box, "COMMS_SCRIPT_TEXT", "");
    script_comms_text->setPosition(20, 30, sp::Alignment::TopLeft)->setSize(760, 500);

    // List possible responses to a scripted communication.
    script_comms_options = new GuiListbox(script_comms_box, "COMMS_SCRIPT_LIST", [this](int index, string value) {
        script_comms_options->setOptions({});
        my_spaceship->commandSendComm(index);
    });
    script_comms_options->setPosition(20, -70, sp::Alignment::BottomLeft)->setSize(700, 400);

    // Button to close scripted comms.
    script_comms_close = new GuiButton(script_comms_box, "CLOSE_BUTTON", tr("button", "Close"), [this]() {
        script_comms_options->setOptions({});
        if (my_spaceship)
            my_spaceship->commandCloseTextComm();
    });
    script_comms_close->setTextSize(20)->setPosition(-20, -20, sp::Alignment::BottomRight)->setSize(150, 50);
}

void GuiCommsOverlay::onUpdate()
{
    // If we're on a ship, show comms activity on draw.
    if (my_spaceship)
    {
        opening_box->setVisible(my_spaceship->isCommsOpening());
        opening_progress->setValue(my_spaceship->getCommsOpeningDelay());

        hailed_box->setVisible(my_spaceship->isCommsBeingHailed());
        hailed_label->setText(tr("Hailed by {name}").format({{"name", my_spaceship->getCommsTargetName()}}));

        no_response_box->setVisible(my_spaceship->isCommsFailed());

        broken_box->setVisible(my_spaceship->isCommsBroken());
        closed_box->setVisible(my_spaceship->isCommsClosed());

        chat_comms_title->setVisible(my_spaceship->isCommsChatOpen());
        if (my_spaceship->getCommsTargetName() != "")
            chat_label->setText(tr("Communication with") + my_spaceship->getCommsTargetName());
        else
            chat_label->setText(tr("Communication"));
        chat_comms_box->setVisible(my_spaceship->isCommsChatOpen() && !minimized);
        chat_comms_text->setText(my_spaceship->getCommsIncommingMessage());
//        chat_comms_message_entry->setFocus(chat_comms_box -> isVisible());
//        chat_comms_message_entry->setActive(chat_comms_box -> isVisible());
//        chat_comms_message_entry->setEnable(chat_comms_box -> isVisible());

        script_comms_box->setVisible(my_spaceship->isCommsScriptOpen());
        script_comms_text->setText(my_spaceship->getCommsIncommingMessage());

        // Show the scripted comms options. If they've changed, update the lsit
        bool changed = script_comms_options->entryCount() != int(my_spaceship->getCommsReplyOptions().size());
        if (!changed && my_spaceship->getCommsReplyOptions().size() > 0)
            changed = my_spaceship->getCommsReplyOptions()[0] != script_comms_options->getEntryName(0);
        if (changed)
        {
            script_comms_options->setOptions({});
            for(string message : my_spaceship->getCommsReplyOptions())
                script_comms_options->addEntry(message, message);
            script_comms_options->setSelectionIndex(-1);
            int display_options_count = std::min(5, script_comms_options->entryCount());
            script_comms_options->setSize(760, display_options_count * 50);
            script_comms_text->setSize(760, 500 - display_options_count * 50);
        }
    }
}

void GuiCommsOverlay::minimize(bool minimize)
{
    chat_minimize_button->setValue(minimize);
    if (minimize != minimized)
    {
        if (minimize)
        {
            chat_comms_box->hide();
//            original_height = getSize().y;
//            setSize(getSize().x, original_height);
        }else{
            chat_comms_box->show();
//            setSize(getSize().x, original_height);
        }
    }
    minimized = minimize;
}

void GuiCommsOverlay::clearElements()
{
    // Force all panels to hide, in case hiding the overlay doesn't hide its
    // contents on draw.
    opening_box->hide();
    hailed_box->hide();
    no_response_box->hide();
    broken_box->hide();
    closed_box->hide();
    chat_comms_title->hide();
    chat_comms_box->hide();
    script_comms_box->hide();
}
