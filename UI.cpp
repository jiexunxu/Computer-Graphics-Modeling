#include "Include.h"

void drawGraphics(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();	
	glOrtho((boundingSphere[0]-boundingSphere[3])*zoom_factor, (boundingSphere[0]+boundingSphere[3])*zoom_factor, (boundingSphere[1]-boundingSphere[3])*zoom_factor, 
		(boundingSphere[1]+boundingSphere[3])*zoom_factor, 0.0, boundingSphere[3]*10);
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(-boundingSphere[0], -boundingSphere[1], -boundingSphere[2]);
	gluLookAt(0.0, 0.0, 10.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glMultMatrixf(x_rotation_matrix);
	glMultMatrixf(y_rotation_matrix);
	glMultMatrixf(z_rotation_matrix);

	glCallList(display_list);
	if(is_wireframe_composite){
		glCallList(wireframe_list);
	}
	
	if(selected_vertices.size()!=0){
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glColor3f(1.0, 0.0, 1.0);
		glPointSize(5.0);
		glBegin(GL_POINTS);
			for(int i=0;i<selected_vertices.size();i++){
				int index=selected_vertices[i];
				glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
			}
		glEnd();
	}

	if((cur_status==Status::RECTANGLE_SELECTION_ACTIVE)||(cur_status==Status::LASSO_SELECTION_ACTIVE)){
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrtho (0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, 0, 1);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity();
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);
			for(int i=0;i<selection_coordinates.size()/2-1;i++){
				glVertex3f(selection_coordinates[2*i], selection_coordinates[2*i+1], 0.0);
				glVertex3f(selection_coordinates[2*i+2], selection_coordinates[2*i+3], 0.0);
			}
		glEnd();
	}
	
/*	glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		glMatrixMode (GL_PROJECTION);
		glLoadIdentity ();
		glOrtho (0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, 0, 1);
		glMatrixMode (GL_MODELVIEW);
		glLoadIdentity();
		glColor3f(1.0, 0.0, 0.0);
	glBegin(GL_POINTS);
	for(int i=0;i<debug.size()/2;i++){
		glVertex3f(debug[i*2], debug[i*2+1], 0.0);
	}
	glEnd();*/

	glFlush();
}

void glutMouseMotionFunction(int x, int y){
	if(cur_status==Status::RECTANGLE_SELECTION_ACTIVE){
		selection_coordinates[3]=y;
		selection_coordinates[4]=x;
		selection_coordinates[5]=y;
		selection_coordinates[6]=x;
	}
	glutPostRedisplay();
}

void glutMouseClickFunction(int button, int button_status, int x, int y){
	if(button==GLUT_LEFT_BUTTON){
		if(button_status==GLUT_DOWN){
			cout<<x<<" "<<y<<"\n";
			if(cur_status==Status::IDLE){
				if(selection_tool_ID==0){
					cur_status=Status::SINGLE_CLICK_SELECTION_ACTIVE;
					selection_coordinates.clear();
					selection_coordinates.push_back(x);
					selection_coordinates.push_back(y);
				}else if(selection_tool_ID==1){
					cur_status=Status::LASSO_SELECTION_ACTIVE;
					selection_coordinates.clear();
					selection_coordinates.push_back(x);
					selection_coordinates.push_back(y);
				}else if(selection_tool_ID==2){
					cur_status=Status::RECTANGLE_SELECTION_ACTIVE;
					selection_coordinates.clear();
					selection_coordinates.push_back(x);
					selection_coordinates.push_back(y);
					selection_coordinates.push_back(x);
					selection_coordinates.push_back(y);
					selection_coordinates.push_back(x);
					selection_coordinates.push_back(y);
					selection_coordinates.push_back(x);
					selection_coordinates.push_back(y);
					selection_coordinates.push_back(x);
					selection_coordinates.push_back(y);
				}
			}else if(cur_status==Status::LASSO_SELECTION_ACTIVE){
				selection_coordinates.push_back(x);
				selection_coordinates.push_back(y);
			}
		}else if(button_status==GLUT_UP){
			if((cur_status==Status::RECTANGLE_SELECTION_ACTIVE)||(cur_status==Status::SINGLE_CLICK_SELECTION_ACTIVE)){
				update_selected_vertices();
				cur_status=Status::IDLE;
			}
		}
	}else if(button==GLUT_RIGHT_BUTTON){
		if(cur_status==Status::LASSO_SELECTION_ACTIVE){
			cur_status=Status::IDLE;
			selection_coordinates.push_back(selection_coordinates[0]);
			selection_coordinates.push_back(selection_coordinates[1]);
			update_selected_vertices();
		}
	}
	glutPostRedisplay();
}

void gluiAndglutInit(int argc, char *argv[]){
	glutInit(&argc, argv);
    glutInitDisplayMode (GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize (600, 600); 
    glutInitWindowPosition (0, 0);
	int window_id=glutCreateWindow ("Mesh_Editor");
	glui=GLUI_Master.create_glui("Control Panel");
	GLUI_Master.set_glutIdleFunc(NULL);
	glui->set_main_gfx_window(window_id);
	GLUI_Panel *display_panel=glui->add_panel("Display Options");
	glui->add_checkbox_to_panel(display_panel, "Wireframe Composite? ", &is_wireframe_composite);
	GLUI_Panel *rotation_panel=glui->add_panel_to_panel(display_panel, "Rotational Controls");
	glui->add_rotation_to_panel(rotation_panel, "X rotation", x_rotation_matrix);
	glui->add_column_to_panel(rotation_panel, false);
	glui->add_rotation_to_panel(rotation_panel, "Y rotation", y_rotation_matrix);
	glui->add_column_to_panel(rotation_panel, false);
	glui->add_rotation_to_panel(rotation_panel, "Z rotation", z_rotation_matrix);
	glutDisplayFunc(drawGraphics); 
	glui->add_separator();
	GLUI_Panel *selection_panel=glui->add_panel("Vertex Selection Options");
	GLUI_Panel *tools_panel=glui->add_panel_to_panel(selection_panel, "Vertex Selection Tools");
	GLUI_RadioGroup *selection_tools=glui->add_radiogroup_to_panel(tools_panel, &selection_tool_ID);
	glui->add_radiobutton_to_group(selection_tools, "Single Click Selection");
	glui->add_radiobutton_to_group(selection_tools, "Lasso Selection Tool");
	glui->add_radiobutton_to_group(selection_tools, "Rectangular Selection Tool");
	GLUI_Panel *boolean_selection_panel=glui->add_panel_to_panel(selection_panel, "Boolean Selection Operations");
	GLUI_RadioGroup *boolean_selections=glui->add_radiogroup_to_panel(boolean_selection_panel, &boolean_selection_ID);
	glui->add_radiobutton_to_group(boolean_selections, "Selection");
	glui->add_radiobutton_to_group(boolean_selections, "Union");
	glui->add_radiobutton_to_group(boolean_selections, "Subtraction");
	GLUI_Master.set_glutMouseFunc(glutMouseClickFunction);
	glutMotionFunc(glutMouseMotionFunction);
}