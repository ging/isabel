/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2011 Agora System S.A.
 * 
 * This file is part of Isabel.
 * 
 * Isabel is free software: you can redistribute it and/or modify
 * it under the terms of the Affero GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Isabel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * Affero GNU General Public License for more details.
 * 
 * You should have received a copy of the Affero GNU General Public License
 * along with Isabel.  If not, see <http://www.gnu.org/licenses/>.
 */

package isabel.argonauta;

import javax.swing.*;
import javax.swing.filechooser.*;
import java.awt.*;
import java.awt.event.*;

/**
 * File browser.
 * 
 * Select documents are open with xdg-open
 */
public class Argonauta {

	JFileChooser fc;

	/**
	 * main method.
	 */
	public static void main(String[] args) {
		Argonauta demo = new Argonauta();
	}

	/**
	 * FileChooserDemo Constructor
	 */
	public Argonauta() {

		fc = new JFileChooser();

		fc.setDragEnabled(false);

		// set the current directory:
		// fc.setCurrentDirectory(swingFile);

		// Add file filters:
		javax.swing.filechooser.FileFilter filter;
		filter = new FileNameExtensionFilter("Images", "jpg", "jpeg", "png",
				"gif");
		fc.addChoosableFileFilter(filter);

		filter = new FileNameExtensionFilter("PDF", "PDF");
		fc.addChoosableFileFilter(filter);

		filter = new FileNameExtensionFilter("Office", "ppt", "pptx", "doc",
				"docx");
		fc.addChoosableFileFilter(filter);

		fc.setAcceptAllFileFilterUsed(true);

		// remove the approve/cancel buttons
		fc.setControlButtonsAreShown(false);

		// Actions & Listener:
		Action openAction = createOpenAction();
		Action dismissAction = createDismissAction();

		fc.addActionListener(openAction);

		// make custom controls
		JPanel buttons = new JPanel();
		buttons.add(new JButton(dismissAction));
		buttons.add(new JButton(openAction));

		// Main Window:

		JFrame jf = new JFrame("File browser");

		jf.setDefaultCloseOperation(WindowConstants.EXIT_ON_CLOSE);

		jf.getContentPane().add(fc, BorderLayout.CENTER);
		jf.getContentPane().add(buttons, BorderLayout.SOUTH);

		jf.pack();
		jf.setVisible(true);
	}

	public Action createOpenAction() {
		return new AbstractAction("Open") {
			public void actionPerformed(ActionEvent e) {
				if (!e.getActionCommand().equals(JFileChooser.CANCEL_SELECTION)
						&& fc.getSelectedFile() != null) {
					openDocument(fc.getSelectedFile().getPath());
				}
			}
		};
	}

	public Action createDismissAction() {
		return new AbstractAction("Dismiss") {
			public void actionPerformed(ActionEvent e) {
				System.exit(0);
			}
		};
	}

	private void openDocument(String name) {

		try {
			Runtime rt = Runtime.getRuntime();

			String[] command = { "xdg-open", name };

			rt.exec(command);
		} catch (Exception e) {
			System.err.println("I can not open this document: " + name);
		}

	}

}
