#include "util/string.hpp"
#include "util/filesys.hpp"
#include "MenuState.hpp"
#include "FileFormat/FileFormat.hpp"
#include "FileFormat/NBE.hpp"
#include "FileFormat/helpers.hpp"
#include "dialogs/FileDialog.hpp"
#include "dialogs/ImageDialog.hpp"
#include "minetest.hpp"
#include <stdlib.h>

#if _WIN32
	#define OPEN_URL(url) system((std::string("start \"\" \"") + std::string(url) + "\"").c_str())
#else
	#define OPEN_URL(url) system((std::string("xdg-open \"") + std::string(url) + "\"").c_str())
#endif

MenuState::MenuState(EditorState* state) :
	state(state),
	projectMenubar(NULL),
	menubar(NULL),
	mode_icons_open(false),
	dialog(NULL)
{}


void MenuState::init()
{
	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	guienv->clear();
	guienv->getSkin()->setFont(guienv->getFont("media/fontlucida.png"));

	// Main menu bar
	menubar = guienv->addMenu();
	menubar->addItem(L"File", -1, true, true);
	menubar->addItem(L"Edit", -1, true, true);
	menubar->addItem(L"View", -1, true, true);
	menubar->addItem(L"Project", -1, true, true);
	menubar->addItem(L"Help", -1, true, true);
	gui::IGUIContextMenu *submenu;

	// File
	submenu = menubar->getSubMenu(0);
	//submenu->addItem(L"New", GUI_FILE_NEW_PROJECT);
	submenu->addItem(L"Open Project", GUI_FILE_OPEN_PROJECT);
	submenu->addItem(L"Save Project", GUI_FILE_SAVE_PROJECT);
	submenu->addItem(L"Save Project As", GUI_FILE_SAVE_PROJECT_AS);
	submenu->addSeparator();
	submenu->addItem(L"Run in Minetest", GUI_FILE_RUN_IN_MINETEST);
	submenu->addItem(L"Export", -1, true, true);
	submenu->addItem(L"Import Nodes", GUI_FILE_IMPORT);
	submenu->addSeparator();
	submenu->addItem(L"Exit", GUI_FILE_EXIT);

	// File > Export
	submenu = submenu->getSubMenu(5);
	submenu->addItem(L"Standalone Lua File (.lua)", GUI_FILE_EXPORT_LUA);
	submenu->addItem(L"Minetest Mod", GUI_FILE_EXPORT_MOD);
	submenu->addItem(L"Current node to mesh (.obj)", GUI_FILE_EXPORT_OBJ);
	submenu->addItem(L"Textures to Folder", GUI_FILE_EXPORT_TEX);

	// Edit
	submenu = menubar->getSubMenu(1);
	submenu->addItem(
		L"Snapping", GUI_EDIT_SNAP, true, false,
		state->settings->getBool("snapping"),
		true
	);
	submenu->addItem(
		L"Limiting", GUI_EDIT_LIMIT, true, false,
		state->settings->getBool("limiting"),
		true
	);

	// View
	submenu = menubar->getSubMenu(2);
	submenu->addItem(L"Tiled View", GUI_VIEW_SP_ALL);
	submenu->addSeparator();
	submenu->addItem(L"Top Left", GUI_VIEW_SP_PER);
	submenu->addItem(L"Top Right", GUI_VIEW_SP_TOP);
	submenu->addItem(L"Bottom Left", GUI_VIEW_SP_FRT);
	submenu->addItem(L"Bottom Right", GUI_VIEW_SP_RHT);

	// Project
	projectMenubar = menubar->getSubMenu(3);
	projectMenubar->addItem(L"Import Image", GUI_PROJ_IMAGE_IM);


	// Help
	submenu = menubar->getSubMenu(4);
	submenu->addItem(L"Help...", GUI_HELP_HELP);
	//submenu->addItem(L"Forum Topic...", GUI_HELP_FORUM);
	submenu->addItem(L"Report bugs...", GUI_HELP_REPORT);
	submenu->addSeparator();
	submenu->addItem(L"About", GUI_HELP_ABOUT);

	// Sidebar root
	u32 top = menubar->getAbsoluteClippingRect().LowerRightCorner.Y;
	sidebar = guienv->addStaticText(L"Loading...",
		rect<s32>(
			state->device->getVideoDriver()->getScreenSize().Width - 246,
			top + 10,
			state->device->getVideoDriver()->getScreenSize().Width,
			state->device->getVideoDriver()->getScreenSize().Height
		), false, true, 0, GUI_SIDEBAR
	);
	sidebar->setAlignment(EGUIA_LOWERRIGHT, EGUIA_LOWERRIGHT, EGUIA_UPPERLEFT, EGUIA_UPPERLEFT);
	sidebar->setNotClipped(true);
}

bool MenuState::OnEvent(const SEvent& event)
{
	if (dialog)
		return dialog->OnEvent(event);

	if (event.EventType == irr::EET_MOUSE_INPUT_EVENT &&
			event.MouseInput.Event == EMIE_LMOUSE_PRESSED_DOWN) {
		if (rect<s32>(10, 32, 42, 64).isPointInside(state->mouse_position)){
			mode_icons_open = !mode_icons_open;
			return true;
		}
		if (mode_icons_open) {
			if (!(rect<s32>(10, 32, 210, 64).isPointInside(state->mouse_position))) {
				mode_icons_open = false;
				return true;
			}
			EditorMode *curs = state->Mode();
			int x = 0;
			for (int i = 0; i < 5; i++){
				EditorMode *m = state->Mode(i);

				if (m && m != curs){
					if (rect<s32>(47 + 37 * x, 32, 79 + 37 * x, 64)
							.isPointInside(state->mouse_position)) {
						state->SelectMode(i);
						mode_icons_open = false;
						return true;
					}
					x++;
				}
			}
		}
	}
	if (event.EventType == EET_GUI_EVENT) {
		if (event.GUIEvent.EventType == EGET_MENU_ITEM_SELECTED) {
			IGUIContextMenu *menu = (IGUIContextMenu *)event.GUIEvent.Caller;
			switch (menu->getItemCommandId(menu->getSelectedItem())){
			case GUI_FILE_OPEN_PROJECT:
				FileDialog_open_project(state);
				return true;
			case GUI_FILE_SAVE_PROJECT:
				if (!state->project) {
					state->device->getGUIEnvironment()->addMessageBox(
							L"Unable to save",
							L"You have not yet opened a project.");
					return true;
				}
				if (state->project->file != "") {
					save_file(getFromType(FILE_FORMAT_NBE, state), state, state->project->file, false);
				} else {
					FileDialog_save_project(state);
				}
				return true;
			case GUI_FILE_SAVE_PROJECT_AS:
				if (!state->project) {
					state->device->getGUIEnvironment()->addMessageBox(
							L"Unable to save",
							L"You have not yet opened a project.");
					return true;
				}
				FileDialog_save_project(state);
				return true;
			case GUI_FILE_RUN_IN_MINETEST: {
				Minetest mt(state->settings);
				if (!mt.findMinetest(state->isInstalled)) {
					state->device->getGUIEnvironment()->addMessageBox(L"Unable to find Minetest",
							L"Minetest could not be found by NBE.\n\t(try setting 'minetest_root' in editor.conf)");
					return true;
				}
				mt.runMod(state);
				return true;
			}
			case GUI_FILE_EXPORT_LUA:
				FileDialog_export(state, FILE_FORMAT_LUA);
				return true;
			case GUI_FILE_EXPORT_MOD:
				FileDialog_export_mod(state);
				return true;
			case GUI_FILE_EXPORT_OBJ:
				FileDialog_export_obj(state, state->project->GetCurrentNode());
				return true;
			case GUI_FILE_EXPORT_TEX:
				FileDialog_export_textures(state);
				return true;
			case GUI_FILE_IMPORT:
				FileDialog_import(state);
				return true;
			case GUI_FILE_EXIT: {
				IGUIEnvironment *guienv = state->device->getGUIEnvironment();
				IGUIWindow *win = guienv->addWindow(rect<irr::s32>(100, 100, 356, 215),
						true, L"Are you sure?");
				guienv->addButton(rect<irr::s32>(128 - 40, 80, 128 + 40, 105),
						win, GUI_FILE_EXIT, L"Close", L"Close the editor");
				return true;
			}
			case GUI_EDIT_SNAP:
				if (menu->isItemChecked(menu->getSelectedItem())) {
					state->settings->set("snapping", "true");
				} else {
					state->settings->set("snapping", "false");
				}

				menu->setItemChecked(menu->getSelectedItem(),
						state->settings->getBool("snapping"));
				return true;
			case GUI_EDIT_LIMIT:
				if (menu->isItemChecked(menu->getSelectedItem())) {
					state->settings->set("limiting", "true");
				} else {
					state->settings->set("limiting", "false");
				}

				menu->setItemChecked(menu->getSelectedItem(),
						state->settings->getBool("limiting"));
				return true;
			case GUI_PROJ_IMAGE_IM:
				ImageDialog::show(state, NULL, ECS_TOP);
				return true;
			case GUI_HELP_HELP:
				OPEN_URL("https://en.wikipedia.org/wiki/Troubleshooting"); // teehee
				break;
			case GUI_HELP_FORUM:
				OPEN_URL("https://forum.minetest.net/viewtopic.php?f=14&t=2840");
				break;
			case GUI_HELP_REPORT:
				OPEN_URL("https://github.com/rollerozxa/NodeBoxEditor/issues");
				break;
			case GUI_HELP_ABOUT: {
				core::stringw msg = L"NodeBoxEditor-ROllerozxa\n"
					L"Version: ";

				msg += EDITOR_TEXT_VERSION_LABEL;
				msg += L"\n\n"
					L"ROllerozxa's fork of rubenwardy's NodeBoxEditor, a program for creating and editing nodeboxes for Minetest.";

				state->device->getGUIEnvironment()->addMessageBox(L"About", msg.c_str());
				return true;
			}
			}
		} else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
			switch (event.GUIEvent.Caller->getID()) {
			case GUI_FILE_EXIT:
				if (state->project) {
					NBEFileFormat writer(state);

					// Get directory to save to
					std::string dir = getSaveLoadDirectory(state->settings->get("save_directory"), state->isInstalled);

					std::cerr << "Saving to " << dir + "exit." << std::endl;
					if (!writer.write(state->project, dir + "exit.nbe"))
						std::cerr << "Failed to save file for unknown reason." << std::endl;
				}
				state->CloseEditor();
				return true;
			}
		}
	} else if (event.EventType == EET_KEY_INPUT_EVENT){
		if (event.KeyInput.Control && event.KeyInput.Key == KEY_KEY_S &&
				!event.KeyInput.PressedDown) {
			if (!state->project) {
				state->device->getGUIEnvironment()->addMessageBox(L"Unable to save",
						L"You have not yet opened a project.");
				return true;
			}
			if (state->project->file != "") {
				save_file(getFromType(FILE_FORMAT_NBE, state), state, state->project->file, false);
			} else {
				FileDialog_save_project(state);
			}
			return true;
		}
	}
	return false;
}

void MenuState::draw(IVideoDriver *driver){
	EditorMode* curs = state->Mode();

	if (state->settings->getBool("hide_sidebar")) {
		sidebar->setVisible(false);
	} else {
		sidebar->setVisible(true);
		u32 top = menubar->getAbsoluteClippingRect().LowerRightCorner.Y;
		state->device->getGUIEnvironment()->getSkin()
			->draw3DWindowBackground(NULL, false, 0,
				rect<s32>((driver->getScreenSize().Width - 256),
					top,
					driver->getScreenSize().Width,
					driver->getScreenSize().Height
				)
			);
	}

	if (dialog)
		return;

	if (curs) {
		driver->draw2DImage(curs->icon(),
			rect<s32>(10, 32, 42, 64),
			rect<s32>( 0,  0, 32, 32),
			0, 0, true);
	}

	if (mode_icons_open) {
		int x = 0;
		for (int i = 0; i < 5; i++) {
			EditorMode* m = state->Mode(i);

			if (m && m != curs) {
				driver->draw2DImage(m->icon(),
					rect<s32>(47 + 37 * x, 32, 79 + 37 * x, 64),
					rect<s32>(0, 0, 32, 32),
					0, 0, true);
				x++;
			}
		}
	}
}
