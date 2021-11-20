#include "NBEditor.hpp"
#include <vector>
#include "../GUIHelpers.hpp"
#include "../util/string.hpp"
#include "../project/node.hpp"
#include "../project/nodebox.hpp"

// The gui id numbers for this mode
// NOTE: the maximum that can be here is 20
//       see in MenuState.hpp to raise the limit
enum NODEBOX_EDITOR_GUI_IDS {
	ENB_GUI_MAIN_LISTBOX = GUI_SIDEBAR + 1,
	ENB_GUI_MAIN_MSG,
	ENB_GUI_PROP,
	ENB_GUI_PROP_X1,
	ENB_GUI_PROP_Y1,
	ENB_GUI_PROP_Z1,
	ENB_GUI_PROP_X2,
	ENB_GUI_PROP_Y2,
	ENB_GUI_PROP_Z2,
	ENB_GUI_PROP_NAME,
	ENB_GUI_PROP_DECIMALS,
	ENB_GUI_PROP_UPDATE,
	ENB_GUI_PROP_REVERT,
	ENB_GUI_FLP_X,
	ENB_GUI_FLP_Y,
	ENB_GUI_FLP_Z,
	ENB_GUI_ROT_X,
	ENB_GUI_ROT_Y,
	ENB_GUI_ROT_Z
};

NBEditor::NBEditor(EditorState* st) :
	EditorMode(st),
	current(-1),
	prop_needs_update(false)
{
	for (int i = 0; i < 4; i++) {
		cdrs[i*5]     = CDR((EViewport)i, CDR_L);
		cdrs[i*5 + 1] = CDR((EViewport)i, CDR_R);
		cdrs[i*5 + 2] = CDR((EViewport)i, CDR_U);
		cdrs[i*5 + 3] = CDR((EViewport)i, CDR_D);
		cdrs[i*5 + 4] = CDR((EViewport)i, CDR_M);
	}
}


void NBEditor::load()
{
	IGUIStaticText* sidebar = state->menu->sidebar;
	IGUIEnvironment* guienv = state->device->getGUIEnvironment();

	if (state->settings->getBool("hide_other_nodes"))
		state->project->hideAllButCurrentNode();
	else
		state->project->remesh();

	if (sidebar) {
		sidebar->setText(L"Node Box Tool");
		IGUIStaticText* t = guienv->addStaticText(L"No node selected",
				rect<s32>(20, 30, 140, 100),
				false, true, sidebar, ENB_GUI_MAIN_MSG);


		IGUIListBox *lb = guienv->addListBox(rect<s32>(10, 35+20, 227, 133+20),
				sidebar, ENB_GUI_MAIN_LISTBOX, true);

		if (lb) {
			lb->setVisible(false);
			IGUIButton* b1 = guienv->addButton(rect<s32>(0, 100, 50, 125),
					lb, GUI_PROJ_NEW_BOX, L"+", L"Add a node box");
			IGUIButton* b2 = guienv->addButton(rect<s32>(60, 100, 110, 125),
					lb, GUI_PROJ_DELETE_BOX, L"-", L"Delete node box");
			IGUIButton* b3 = guienv->addButton(rect<s32>(120, 100, 170, 125),
					lb, GUI_PROJ_CLONE, L"Clone", L"Duplicate node box");
			b1->setNotClipped(true);
			b2->setNotClipped(true);
			b3->setNotClipped(true);
		}


		// Rotate X
		static ITexture *flp_x = state->device->getVideoDriver()->getTexture("media/flip_x.png");
		IGUIButton *btn = guienv->addButton(rect<s32>(10, 20, 120+32 - (32+5)*3, 52), sidebar, ENB_GUI_FLP_X, L"", L"Rotate node through X axis");
		btn->setImage(flp_x);
		btn->setUseAlphaChannel(true);

		// Rotate Y
		static ITexture *flp_y = state->device->getVideoDriver()->getTexture("media/flip_y.png");
		btn = guienv->addButton(rect<s32>(121 - (32+5)*2, 20, 120+32 - (32+5)*2, 32+20), sidebar, ENB_GUI_FLP_Y, L"", L"Rotate node through Y axis");
		btn->setImage(flp_y);
		btn->setUseAlphaChannel(true);

		// Rotate Z
		static ITexture *flp_z = state->device->getVideoDriver()->getTexture("media/flip_z.png");
		btn = guienv->addButton(rect<s32>(121 - (32+5), 20, 120+32 - (32+5), 32+20), sidebar, ENB_GUI_FLP_Z, L"", L"Rotate node through Z axis");
		btn->setImage(flp_z);
		btn->setUseAlphaChannel(true);

		// Rotate X
		static ITexture *rot_x = state->device->getVideoDriver()->getTexture("media/rotate_x.png");
		btn = guienv->addButton(rect<s32>(121, 20, 120+32, 32+20), sidebar, ENB_GUI_ROT_X, L"", L"Rotate node through X axis");
		btn->setImage(rot_x);
		btn->setUseAlphaChannel(true);

		// Rotate Y
		static ITexture *rot_y = state->device->getVideoDriver()->getTexture("media/rotate_y.png");
		btn = guienv->addButton(rect<s32>(121 + (32+5)*1, 20, 120+32 + (32+5)*1, 32+20), sidebar, ENB_GUI_ROT_Y, L"", L"Rotate node through Y axis");
		btn->setImage(rot_y);
		btn->setUseAlphaChannel(true);

		// Rotate Z
		static ITexture *rot_z = state->device->getVideoDriver()->getTexture("media/rotate_z.png");
		btn = guienv->addButton(rect<s32>(121 + (32+5)*2, 20, 120+32 + (32+5)*2, 32+20), sidebar, ENB_GUI_ROT_Z, L"", L"Rotate node through Z axis");
		btn->setImage(rot_z);
		btn->setUseAlphaChannel(true);

		// Create nodebox properties
		t = guienv->addStaticText(L"Properties",
				rect<s32>(0, 170+20, 120, 190+20),
				false, true, sidebar, ENB_GUI_PROP);
		t->setVisible(false);

		// Add name properties box
		guienv->addStaticText(L"Name:", rect<s32>(10, 30, 50, 50), false,
				true, t)->setNotClipped(true);
		guienv->addEditBox(L"", rect<s32>(60, 30, 210, 50), true,
				t, ENB_GUI_PROP_NAME)->setNotClipped(true);

		// Add positioning
		addXYZ(t, guienv, vector2di(10, 60),  ENB_GUI_PROP_X1);
		addXYZ(t, guienv, vector2di(10, 140), ENB_GUI_PROP_X2);

		// Add show decimals checkbox
		bool fp = state->settings->getBool("fractional_positions");
		guienv->addCheckBox(fp, rect<s32>(30, 215, 200, 245), t,
				ENB_GUI_PROP_DECIMALS, L"As multiples of 1/16")->setNotClipped(true);

		// Add buttons
		guienv->addButton(rect<s32>(30, 250, 100, 250+30), t, ENB_GUI_PROP_UPDATE,
				L"Update", L"")->setNotClipped(true);
		guienv->addButton(rect<s32>(110, 250, 180, 250+30), t, ENB_GUI_PROP_REVERT,
				L"Revert", L"")->setNotClipped(true);

	}
	load_ui();
}

void NBEditor::load_ui()
{
	IGUIStaticText *sidebar = state->menu->sidebar;

	if (!sidebar) {
		return;
	}

	IGUIEnvironment *guienv = state->device->getGUIEnvironment();
	Node *node = state->project->GetCurrentNode();
	if (!node) {
		sidebar->getElementFromId(ENB_GUI_MAIN_MSG)->setVisible(true);
		sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX)->setVisible(false);
		sidebar->getElementFromId(ENB_GUI_PROP)->setVisible(false);
	} else {
		IGUIListBox *lb = (IGUIListBox *) sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);
		sidebar->getElementFromId(ENB_GUI_MAIN_MSG)->setVisible(false);
		sidebar->getElementFromId(ENB_GUI_PROP)->setVisible(false);

		if (lb) {
			lb->clear();
			lb->setVisible(true);
			std::vector<NodeBox*> & boxes = node->boxes;
			for (std::vector<NodeBox*>::const_iterator it = boxes.begin();
					it != boxes.end();
					++it) {
				lb->addItem(narrow_to_wide((*it)->name).c_str());
			}
			lb->setSelected(lb->getListItem(node->GetId()));
		}

		fillProperties();
	}
}

void NBEditor::fillProperties()
{
	IGUIStaticText *sidebar = state->menu->sidebar;
	Node *node = state->project->GetCurrentNode();

	if (!sidebar || !node)
		return;

	NodeBox *nb = node->GetCurrentNodeBox();

	if (!nb)
		return;

	sidebar->getElementFromId(ENB_GUI_PROP)->setVisible(true);

	vector3df one = nb->one;
	vector3df two = nb->two;

	if (state->settings->getBool("fractional_positions")) {
		one *= 16;
		two *= 16;
	}

	fillTB(sidebar, ENB_GUI_PROP, ENB_GUI_PROP_X1, one.X);
	fillTB(sidebar, ENB_GUI_PROP, ENB_GUI_PROP_Y1, one.Y);
	fillTB(sidebar, ENB_GUI_PROP, ENB_GUI_PROP_Z1, one.Z);
	fillTB(sidebar, ENB_GUI_PROP, ENB_GUI_PROP_X2, two.X);
	fillTB(sidebar, ENB_GUI_PROP, ENB_GUI_PROP_Y2, two.Y);
	fillTB(sidebar, ENB_GUI_PROP, ENB_GUI_PROP_Z2, two.Z);

	IGUIElement* element = sidebar->getElementFromId(ENB_GUI_PROP)
			->getElementFromId(ENB_GUI_PROP_NAME);

	if (element) {
		IGUIEditBox* editbox = static_cast<IGUIEditBox*>(element);

		std::wstring wide = narrow_to_wide(nb->name);
		editbox->setText(wide.c_str());
	}
}


void NBEditor::unload()
{}


void NBEditor::update(double dtime)
{
	if (state->menu->dialog)
		current = -1;

	wasmd = state->mousedown;
}


void NBEditor::drawViewport(irr::video::IVideoDriver *driver, EViewport viewport,
		rect<s32> area)
{
	if (wasmd && !state->mousedown)
		current = -1;

	static irr::video::ITexture *scale = driver->getTexture("media/gui_scale.png");

	for (int i = 0; i < 20; i++) {
		if (cdrs[i].visible && cdrs[i].window == viewport) {
			if (area.isPointInside(cdrs[i].position)) {
				rect<s32> drawarea = rect<s32>(
						cdrs[i].position.X - 5,
						cdrs[i].position.Y - 5,
						cdrs[i].position.X + 5,
						cdrs[i].position.Y + 5
				);

				if (!wasmd && state->mousedown &&
						drawarea.isPointInside(state->mouse_position)) {
					current = i;
				}

				SColor color = SColor(255, 255, 255, 255);

				if (current == i) {
					color = SColor(255, 0, 0, 255);
				} else if (state->keys[KEY_LCONTROL] == EKS_DOWN) {
					color = SColor(255, 255, 255, 0);
				}

				driver->draw2DImage(
					scale,
					drawarea.UpperLeftCorner,
					rect<s32>(0, 0, 10, 10),
					NULL, color, true
				);

				cdrs[i].visible = false;
			} // end if point is inside
		} // end if cdr is to be drawn
	} // end for
}


void NBEditor::viewportTick(EViewport window, irr::video::IVideoDriver* driver, rect<s32> offset)
{
	for (int i = 0; i < 20; i++) {
		if (cdrs[i].window == window) {
			cdrs[i].update(this, (current == i), offset);
		}
	}
}

ECDR_DIR CDR::getActualType(EditorState* state)
{
	EViewportType vpt = state->getEViewportType(window);

	static ECDR_DIR crosstable[15] = {
		CDR_X_N, CDR_X_P, CDR_Y_P, CDR_Y_N, CDR_XY,	// front / back
		CDR_Z_N, CDR_Z_P, CDR_Y_P, CDR_Y_N, CDR_ZY,	// left / right
		CDR_X_N, CDR_X_P, CDR_Z_P, CDR_Z_N, CDR_XZ	// top / bottom
	};

	if (vpt == VIEWT_PERS)
		return CDR_NONE;
	else
		return crosstable[(ECDR_DIR)( ( ((int)vpt - 1) % 3) * 5 + type)];
}

void CDR::update(NBEditor* editor, bool drag, rect<s32> offset)
{
	if (!editor->state->project) {
		return;
	}

	ECDR_DIR actualType = getActualType(editor->state);
	if (actualType == CDR_NONE)
		return;

	if (!editor->state->settings->getBool("always_show_position_handle") &&
			(editor->state->keys[KEY_LSHIFT] == EKS_UP) ==
			(actualType == CDR_XY || actualType == CDR_XZ || actualType == CDR_ZY)) {
		return;
	}

	Node* node = editor->state->project->GetCurrentNode();

	if (!node) {
		return;
	}

	NodeBox *box = node->GetCurrentNodeBox();

	if (!box) {
		return;
	}

	ISceneManager * smgr = editor->state->device->getSceneManager();

	if (drag) {
		// get mouse position
		position2di target = editor->state->mouse_position;
		target -= offset.UpperLeftCorner;

		// Get the ray
		line3d<f32> ray = smgr->getSceneCollisionManager()
				->getRayFromScreenCoordinates(target, smgr->getActiveCamera());

		// Contains the output values
		vector3df wpos = vector3df(0, 0, 0); // The collision position

		// Not used, but required for function
		ISceneNode* tmpNode;
		triangle3df tmpTri;

		// Execute ray
		smgr->getSceneCollisionManager()->getCollisionPoint(ray,
				editor->state->plane_tri, wpos, tmpTri, tmpNode);

		// Snapping
		wpos -= vector3df(
			(f32)node->position.X,
			(f32)node->position.Y,
			(f32)node->position.Z
		);

		// Do node limiting
		if (editor->state->settings->getBool("limiting")) {
			// X Axis
			if (wpos.X < -0.5) {
				wpos.X = -0.5;
			} else if (wpos.X > 0.5) {
				wpos.X = 0.5;
			}

			// Y Axis
			if (wpos.Y < -0.5) {
				wpos.Y = -0.5;
			} else if (wpos.Y > 0.5) {
				wpos.Y = 0.5;
			}

			// Z Axis
			if (wpos.Z < -0.5) {
				wpos.Z = -0.5;
			} else if (wpos.Z > 0.5) {
				wpos.Z = 0.5;
			}
		}

		// Call required function
		editor->fillProperties();

		int snap_res = node->snap_res;
		if (snap_res == -1) {
			static const int set_snap_res =
				editor->state->settings->getInt("default_snap_res");
			snap_res = set_snap_res;
		}

		if (actualType < CDR_XZ) {
			if (editor->state->settings->getBool("snapping")) {
				wpos.X = (f32)floor((wpos.X + 0.5) * snap_res + 0.5) / snap_res - 0.5;
				wpos.Y = (f32)floor((wpos.Y + 0.5) * snap_res + 0.5) / snap_res - 0.5;
				wpos.Z = (f32)floor((wpos.Z + 0.5) * snap_res + 0.5) / snap_res - 0.5;

				// (X + 0.5)   round relative to (-0.5, -0.5, -0.5)
				//             ie: (-0.5, -0.5, -0.5) goes to (0, 0, 0)
				// * 16        round to the nearest 16th
				// + 0.5       do rounding, rather than flooring
				// / 16 - 0.5  go back to normal coordinates
			}

			box->moveFace(editor->state, actualType, wpos,
				editor->state->keys[KEY_LCONTROL] == EKS_DOWN);
		} else {
			box->move(editor->state, actualType, wpos,
				(editor->state->settings->getBool("snapping")?snap_res:0));
		}
		node->remesh(box);

		editor->triggerCDRmoved();
	}

	vector3df pos = box->GetCenter();

	switch (actualType) {
	case CDR_X_P:
		pos.X = box->two.X;
		break;
	case CDR_X_N:
		pos.X = box->one.X;
		break;
	case CDR_Y_P:
		pos.Y = box->two.Y;
		break;
	case CDR_Y_N:
		pos.Y = box->one.Y;
		break;
	case CDR_Z_P:
		pos.Z = box->two.Z;
		break;
	case CDR_Z_N:
		pos.Z = box->one.Z;
		break;
	}

	pos.X += node->position.X;
	pos.Y += node->position.Y;
	pos.Z += node->position.Z;

	vector2d<irr::s32> cpos = smgr->getSceneCollisionManager()
			->getScreenCoordinatesFrom3DPosition(
				pos,
				smgr->getActiveCamera()
				, true
			);

	position = cpos + offset.UpperLeftCorner;
	visible = true;
}

bool NBEditor::OnEvent(const irr::SEvent &event) {
	if (event.EventType == EET_GUI_EVENT) {
		if (event.GUIEvent.EventType == EGET_CHECKBOX_CHANGED) {
			switch (event.GUIEvent.Caller->getID()) {
			case ENB_GUI_PROP_DECIMALS: {
				bool fp = state->settings->getBool("fractional_positions");
				state->settings->set("fractional_positions",
						fp?"false":"true");
				fillProperties();
				return true;
			}}
		} else if (event.GUIEvent.EventType == EGET_BUTTON_CLICKED) {
			switch (event.GUIEvent.Caller->getID()) {
			case GUI_PROJ_NEW_BOX: {
				Node* node = state->project->GetCurrentNode();
				if (node) {
					node->addNodeBox();
					load_ui();
				}
				break;
			}
			case GUI_PROJ_DELETE_BOX: {
				Node* node = state->project->GetCurrentNode();
				IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);
				if (node && node->GetNodeBox(lb->getSelected())){
					node->deleteNodebox(lb->getSelected());
					load_ui();
				}
				break;
			}
			case GUI_PROJ_CLONE: {
				Node* node = state->project->GetCurrentNode();
				IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);
				if (node && node->GetNodeBox(lb->getSelected())){
					node->cloneNodebox(lb->getSelected());
					load_ui();
				}
				break;
			}
			case ENB_GUI_PROP_REVERT:
				fillProperties();
				break;
			case ENB_GUI_PROP_UPDATE:
				updateProperties();
				break;
			case ENB_GUI_ROT_X: {
				Node* node = state->project->GetCurrentNode();
				if (node)
					node->rotate(EAX_X);
				break;
			}
			case ENB_GUI_ROT_Y: {
				Node* node = state->project->GetCurrentNode();
				if (node)
					node->rotate(EAX_Y);
				break;
			}
			case ENB_GUI_ROT_Z: {
				Node* node = state->project->GetCurrentNode();
				if (node)
					node->rotate(EAX_Z);
				break;
			}
			case ENB_GUI_FLP_X: {
				Node* node = state->project->GetCurrentNode();
				if (node)
					node->flip(EAX_X);
				break;
			}
			case ENB_GUI_FLP_Y: {
				Node* node = state->project->GetCurrentNode();
				if (node)
					node->flip(EAX_Y);
				break;
			}
			case ENB_GUI_FLP_Z: {
				Node* node = state->project->GetCurrentNode();
				if (node)
					node->flip(EAX_Z);
				break;
			}}
		} else if (event.GUIEvent.EventType == EGET_LISTBOX_CHANGED) {
			Node* node = state->project->GetCurrentNode();
			IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);
			if (node && lb && node->GetNodeBox(lb->getSelected())){
				node->select(lb->getSelected());
			}
			load_ui();
		}
	} else if (event.EventType == EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown) {
		if (event.KeyInput.Key == KEY_RETURN) {
			updateProperties();
			return false;
		}

		if (
				state->device->getGUIEnvironment()->getFocus() &&
				state->device->getGUIEnvironment()->getFocus()->getType() == EGUIET_EDIT_BOX) {
			return false;
		}

		if (event.KeyInput.Key == KEY_INSERT) {
			Node *node = state->project->GetCurrentNode();
			if (node) {
				node->addNodeBox();
				load_ui();
			}
		} else if (event.KeyInput.Key == KEY_DELETE) {
			Node* node = state->project->GetCurrentNode();
			IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);
			if (node && node->GetNodeBox(lb->getSelected())) {
				node->deleteNodebox(lb->getSelected());
			}
			load_ui();
		} else if (event.KeyInput.Key == KEY_DOWN) {
			IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);
			Node* node = state->project->GetCurrentNode();
			if (node) {
				int idx = node->GetId();
				if (lb && idx < (int)node->boxes.size() - 1){
					node->select(idx + 1);
				}
			}
			load_ui();
		} else if (event.KeyInput.Key == KEY_UP) {
			IGUIListBox* lb = (IGUIListBox*) state->menu->sidebar->getElementFromId(ENB_GUI_MAIN_LISTBOX);
			Node* node = state->project->GetCurrentNode();
			if (node) {
				int idx = node->GetId();
				if (lb && idx > 0) {
					node->select(idx - 1);
				}
			}
			load_ui();
		}
	}
	return false;
}

void NBEditor::updateProperties()
{
	IGUIElement* prop = state->menu->sidebar->getElementFromId(ENB_GUI_PROP);
	Node* node = state->project->GetCurrentNode();
	if (!prop || !node) {
		return;
	}

	NodeBox* nb = node->GetCurrentNodeBox();
	if (!nb) {
		return;
	}

	try {
		irr::core::stringc name = prop->getElementFromId(ENB_GUI_PROP_NAME)->getText();
		nb->name = str_replace(std::string(name.c_str(), name.size()), ' ', '_');
		vector3df one(
			(f32)wcstod(prop->getElementFromId(ENB_GUI_PROP_X1)->getText(), NULL),
			(f32)wcstod(prop->getElementFromId(ENB_GUI_PROP_Y1)->getText(), NULL),
			(f32)wcstod(prop->getElementFromId(ENB_GUI_PROP_Z1)->getText(), NULL)
		);
		vector3df two(
			(f32)wcstod(prop->getElementFromId(ENB_GUI_PROP_X2)->getText(), NULL),
			(f32)wcstod(prop->getElementFromId(ENB_GUI_PROP_Y2)->getText(), NULL),
			(f32)wcstod(prop->getElementFromId(ENB_GUI_PROP_Z2)->getText(), NULL)
		);

		if (state->settings->getBool("fractional_positions")) {
			one /= 16;
			two /= 16;
		}

		if (one != nb->one || two != nb->two) {
			nb->one = one;
			nb->two = two;
			nb->rebuild_needed = true;
		}
		node->remesh();
		load_ui();
	} catch(void* e) {
		state->device->getGUIEnvironment()->addMessageBox(L"Update failed",
				L"Please check that the properties contain only numbers.");
	}
}

irr::video::ITexture* NBEditor::icon()
{
	static irr::video::ITexture* icon = state->device->getVideoDriver()
			->getTexture("media/icon_mode_nodebox.png");
	return icon;
}
