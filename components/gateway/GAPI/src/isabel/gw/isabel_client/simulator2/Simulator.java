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
package isabel.gw.isabel_client.simulator2;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import isabel.gw.*;
import isabel.gw.isabel_client.ShutdownEvent;

import isabel.lib.Logs;

import java.util.logging.*;

/**
 * Simulator of a program using the IsabelClient classes.
 * 
 */
public class Simulator implements IsabelStateListener {

    /**
     * Logs
     */
    private Logger mylogger;
    
    private IsabelState is;

    /**
     * Widget where traces are displayed.
     */
    private JTextArea traces;

	/**
	 * Scroll panel to used with traces.
	 */
	JScrollPane jsp;
	
	/**
	 * Construct the simulator object.
	 */
	public Simulator() {

		mylogger = Logger
				.getLogger("isabel.gw.isabel_client.simulato2.Simulator");

		mylogger.info("Creating The Simulator.");

		makeGUI();

		new SeCoMonitor(this).killOnSeCoExit();

		IsabelState.getIsabelState().addIsabelStateListener(this);
	}

	/**
	 * Create the gui.
	 */
	void makeGUI() {

		JFrame jf = new JFrame("Gateway Simulator");
		jf.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

		// Title

		JLabel jl = new JLabel("<html><font size=+1>Gateway Simulator</font>",
				SwingConstants.CENTER);
		jf.getContentPane().add(BorderLayout.NORTH, jl);

		// Panel 1:
		JPanel jp1 = new JPanel();
		jf.getContentPane().add(BorderLayout.CENTER, jp1);
		jp1.setLayout(new BorderLayout());
		// Panel 12:
		JPanel jp12 = new JPanel();
		jp1.add(BorderLayout.NORTH, jp12);

		// Nickname:
		JLabel lnickname = new JLabel("Nick:");
		final JTextField tnickname = new JTextField(5);
		jp12.add(lnickname);
		jp12.add(tnickname);

		// Address:
		JLabel laddr = new JLabel("Addr:");
		final JTextField taddr = new JTextField(10);
		jp12.add(laddr);
		jp12.add(taddr);

		// flows:
		JLabel lpayloads = new JLabel("PT:");
		final JTextField tpayloads = new JTextField(20);
		jp12.add(lpayloads);
		jp12.add(tpayloads);

		// Traces:
		traces = new JTextArea();
		jsp = new JScrollPane(traces);
		jsp.setPreferredSize(new Dimension(320, 200));
		jp1.add(BorderLayout.CENTER, jsp);

		// Create New Site:
		JButton jnew = new JButton("Create a New Site");
		jnew.addActionListener(new ActionListener() {

			public void actionPerformed(ActionEvent ae) {

				String nickname = tnickname.getText();
				String addr = taddr.getText();

				String payloads = tpayloads.getText();

				createNewSite(nickname, addr, payloads);
			}
		});
		jf.getContentPane().add(BorderLayout.SOUTH, jnew);

		// Make visible:
		jf.pack();
		jf.setVisible(true);
	}

	/**
	 * Write a new trace
	 */
	void trace(String msg) {
		
		traces.append(msg+"\n");

		JScrollBar jsb = jsp.getVerticalScrollBar();
		jsb.setValue(jsb.getMaximum()); 
		
		Thread.yield();
	}

	/**
	 * Create a new Site
	 */
	void createNewSite(String nickname, String addr, String payloads) {

		mylogger.info("Create a new site: " + nickname + " " + addr + " "
				+ payloads);

		new Client(nickname, addr, payloads);

	}

	// LISTENER:

	/**
	 * IsabelClient calls this method every time an Isabel member joins the
	 * session.
	 * 
	 * @return Result
	 * @param event
	 */
	public synchronized Result newIsabelSite(IsabelClientEvent event) {

		trace("-> newIsabelSite " + event.getMemberInfo());

		return null;
	}

	/**
	 * IsabelClient calls this method every time a member (external or internal)
	 * is disconnected.
	 * 
	 * @return Result
	 * @param event
	 */
	public synchronized Result siteDisconnection(IsabelClientEvent event) {
		trace("-> siteDisconnection " + event.getMemberInfo());
		return null;
	}

	/**
	 * IsabelClient call this method every time an external node connects.
	 * 
	 * @return Result
	 * @param event
	 */
	public synchronized Result siteReady(IsabelClientEvent event) {
		trace("-> siteReady " + event.getMemberInfo());

		return null;
	}

	/**
	 * IsabelClient calls this method to inform the listeners about which flows
	 * should sent the external site to the isabel session
	 * 
	 * @param event
	 *            Specifies a site and the flows that should send to the isabel
	 *            session.
	 * 
	 * @return A result object.
	 */
	public synchronized Result activeFlows(IsabelClientEvent event) {
		trace("-> activeFlows " + event.getMemberInfo());
		return null;
	}

	/**
	 * IsabelClient calls this method when the connection with the isabel
	 * session is lost, or it can not be established.
	 * 
	 * @param event
	 */
	public synchronized void shutdown(ShutdownEvent event) {
		trace("-> shutdown " + event.getReason());

	}

	/**
	 * IsabelClient calls this method to inform the listeners about changes in the session,
	 * such as the application of a new interaction mode.
	 * 
	 * @param event Describe the change and provides te time of the event.
	 */
	public void isabelChange(SessionEvent event) {
		trace("-> isabelChange " + event.toString());
	}
	
	@Override
	public void parameterChange(String user, String name, Object value) {
		trace("-> parameterChange(" + user + "," + name + "," + value + ")");
	}
	
	// -----------------------------------------------------------------------

	public static void main(String[] args) {

		// Inicializacion de los logs.
		Logs.configure("IsabelStateSimulator");

		new isabel.gw.isabel_client.simulator2.Simulator();

	}

}
