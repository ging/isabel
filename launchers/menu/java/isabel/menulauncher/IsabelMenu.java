/*
 * ISABEL: A group collaboration tool for the Internet
 * Copyright (C) 2009 Agora System S.A.
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

/*
 * IsabelMenu.java
 */

package isabel.menulauncher;

import isabel.lib.Logs;
import isabel.lib.StringParser;
import java.io.*;
import java.awt.event.*;
import java.awt.*;
import javax.swing.*;

import java.util.logging.*;
//import java.util.Arrays;

/**
 * Menu launcher for the Isabel tools.
 */
public class IsabelMenu {
    

    /**
     * Logger where logs are written.
     */
    private Logger mylogger;
    
    /**
     * Creates a new instance of IsabelMenu
     * 
     * @throws IOException IOException created by the configurtation files.
     */
    public IsabelMenu() throws IOException {
	
	mylogger = Logger.getLogger("isabel.menulauncher.IsabelMenu");
	mylogger.info("Creating Isabel Menu.");
		
	// Main window. It is not visible.
	JFrame aux = new JFrame("Isabel");
	aux.setLocation(-100, -100);
	aux.pack();
	aux.setVisible(false);


	// Dialog (without decoration) to show the menu button to open the popup menus.
	JDialog frame = new JDialog(aux);
	frame.setUndecorated(true);


	// The Quit popup menu: a menu with one menu entry to quit this application.
	final JPopupMenu quitMenu = new JPopupMenu();
	JMenuItem quit = new JMenuItem("Quit");
	quit.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    System.exit(0);
		}
	    });
	quitMenu.add(quit);
    

	// The Actions popup menu: a menu with the commands to execute (defined in the menuconfig directory).
	final JPopupMenu actionsMenu = new JPopupMenu();

	// Loads and parser the menu configuration file.
	File configFile = new File(System.getenv("ISABEL_DIR") + "/lib/menu.config");
	BufferedReader in = new BufferedReader(new FileReader(configFile));
	StringBuffer fullConfig = new StringBuffer();  
	String line;
	while ((line = in.readLine()) != null) {
	    fullConfig.append(" " + line);
	}
	in.close();
	parseConfig(actionsMenu,fullConfig.toString());


	// Button to open the popup menus.
	JButton topb = new JButton();
	topb.setMargin(new Insets(0, 0, 0, 0));
	topb.setFont(new Font("serif", Font.BOLD, 17));
	// topb.setIcon(new ImageIcon(text.substring(1)));
	topb.setText("Isabel");
	topb.addMouseListener(new MouseAdapter() {
		public void mousePressed(MouseEvent e) {
		    if (e.getButton() == MouseEvent.BUTTON1) {
			actionsMenu.show(e.getComponent(), 0, e.getComponent().getHeight());
		    }
		    if (e.getButton() == MouseEvent.BUTTON3) {
			quitMenu.show(e.getComponent(), 0, e.getComponent().getHeight());
		    }
		}
	    });
	
	frame.getContentPane().add(topb);

	frame.pack();
	
	Dimension screendim = Toolkit.getDefaultToolkit().getScreenSize();
	
	int posx = (int) (screendim.getWidth() - frame.getSize().getWidth() - 10);
	int posy = (int) (screendim.getHeight() - frame.getSize().getHeight() - 80);
	
	frame.setLocation(posx, posy);
    
	frame.setVisible(true);
    }
    
  
    /**
     * Parse the given configuration file, and update the given menu.
     */
    private void parseConfig(JComponent parentMenu, String config) {
	
	try {
	    StringParser sp = new StringParser(config);
	    for (int i = 0; i < sp.size(); i++) {
		String sval = sp.tokenAt(i);

		mylogger.info("Parsing line: "+ sval);

		StringParser sp2 = new StringParser(sval);

		String type = sp2.tokenAt(0);

		if (type.equals("item")) {
		    String title = sp2.tokenAt(1);
		    String icon = sp2.tokenAt(2);
		    String cmd = sp2.tokenAt(3);
		    parentMenu.add(createMenuItem(title, icon, cmd));
		    
		} else if (type.equals("menu")) {
		    String title = sp2.tokenAt(1);
		    String icon = sp2.tokenAt(2);
		    String submenuConfig = sp2.tokenAt(3);
    
		    JMenu submenu = new JMenu(title);
		    submenu.setIcon(getIcon(icon));

		    parseConfig(submenu,submenuConfig);

		    parentMenu.add(submenu);

		} else {
		    mylogger.warning("Unknown line type:"+ type);
		}
	    }

	} catch (Exception e) {
	    mylogger.severe("Parsing the config menu file, I get: "+e.getMessage());
	}
    }


    /**
     * Create a 32x32 icon.
     */
    private ImageIcon getIcon(String filename) {

	Image img = Toolkit.getDefaultToolkit().getImage(filename).getScaledInstance(32,32,Image.SCALE_SMOOTH);

	return new ImageIcon(img);
    }

    
    /**
     * Creates a JMenuItem.
     * @param name The title of the JMenuItem
     * @param icon Filename of the icon of the JMenuItem
     * @param command The command to execute when selected.
     */
    private JComponent createMenuItem(String name, String icon, String command) {

	JMenuItem item = new JMenuItem();

	item.setActionCommand(command);
	item.setText(name);
	item.setIcon(getIcon(icon));
	
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent aev) {
		    execCommand(aev);
		}
	    });

	return item;
    }

    
    /** 
     * Execute the action command of the menu item of the given action event. 
     */
    private void execCommand(ActionEvent aev) {
	String action = ((JMenuItem) aev.getSource()).getActionCommand();
	mylogger.info("Executing: " + action);
	try {
	    // Command parsing
	    StringParser sp = new StringParser(action);
	    String[] command = new String[sp.size()];
	    for (int i = 0; i < command.length; i++) {
		command[i] = sp.tokenAt(i);
	    }
	    
	    Runtime rt = Runtime.getRuntime();
	    final Process process = rt.exec(command);
	    
	    // Redirect to the standard output.
	    new Thread() {
		    
		    public void run() {
			BufferedReader br = new BufferedReader(new InputStreamReader(process.getInputStream()));
			try {
			    String msg;
			    while ((msg = br.readLine()) != null) {
				System.out.println(msg);
			    }
			} catch (Exception e) {
			}
			try {
			    br.close();
			} catch (Exception e) {
			}
		    }
		    
		}.start();
	    
	    // Redirect to the error output.
	    new Thread() {
		    
		    public void run() {
			BufferedReader br = new BufferedReader(new InputStreamReader(process.getErrorStream()));
			try {
			    String msg;
			    while ((msg = br.readLine()) != null) {
				System.err.println(msg);
			    }
			} catch (Exception e) {
			}
			try {
			    br.close();
			} catch (Exception e) {
			}
		    }
		    
		}.start();
	    
	} catch (Exception e) {
	    mylogger.warning("Executing \"" + action + "\", I get: "
			     + e.getMessage());
	}
    }
    
    
    /**
     * Main method of the application.
     */
    public static void main(String[] args) {
	
	// Inicializacion de los logs.
	Logs.configure("isabelmenu");
	
	try {
	    new IsabelMenu();
	} catch (IOException e) {
	    System.err.println("Fatal error. \", I get: " + e.getMessage());
	}
    }
}
