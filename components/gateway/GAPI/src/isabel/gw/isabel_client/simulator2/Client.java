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

import isabel.gw.FlowInfo;
import isabel.gw.IsabelClient;
import isabel.gw.MemberInfo;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.StringTokenizer;
import java.util.Vector;
import java.util.logging.Logger;

import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.SwingConstants;

public class Client {

	/**
	 * Logs
	 */
	private Logger mylogger;

	/**
	 * The IsabelClient object which connect with the isabel session.
	 */
	IsabelClient ic;

	/**
	 * Widget where traces are displayed.
	 */
	private JTextArea traces;

	/**
	 * Scroll panel to used with traces.
	 */
	JScrollPane jsp;
	
	/**
	 * Constructor
	 */
	Client(String nickname, String addr, String payloads) {

		mylogger = Logger.getLogger("isabel.gw.isabel_client.simulato2.Client");
		mylogger.info("Created the " + nickname + " client.");

		makeGUI(nickname);

		Thread.yield();
		
		ic = IsabelClient.create();

		trace("IsabelClient Created");

		Vector<FlowInfo> flowList = new Vector<FlowInfo>();

		StringTokenizer st = new StringTokenizer(payloads);
		while (st.hasMoreTokens()) {
			String spt = st.nextToken();
			try {
				int pt = Integer.parseInt(spt);
				FlowInfo fi = new FlowInfo(pt, 10, 20, -1);
				flowList.add(fi);
			} catch (NumberFormatException nfe) {
				System.err.println("Invalid payload type: " + spt);
			}
		}

		MemberInfo mi = new MemberInfo(0, nickname, addr, flowList);

		trace("Calling IsabelClient.connect(" + nickname + "," + addr + "," + flowList
				+ ")");

		ic.connect(mi);

	}

	/**
	 * Disconnect
	 */
	void disconnect() {

		trace("Calling IsabelClient.disconnect()");

		ic.disconnect();
	}

	/**
	 * Create the gui.
	 */
	void makeGUI(String title) {
 
		JFrame jf = new JFrame("Client " + title);
		jf.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

		// Title

		JLabel jl = new JLabel("<html><font size=+1>Client "+title+"</font>",
				SwingConstants.CENTER);
		jf.getContentPane().add(BorderLayout.NORTH, jl);

		// Traces:
		traces = new JTextArea();
		jsp = new JScrollPane(traces);
		jsp.setPreferredSize(new Dimension(320, 200));
		jf.getContentPane().add(BorderLayout.CENTER, jsp);

		// Bottom panel
		JPanel jpb = new JPanel();
		jpb.setLayout(new FlowLayout());
		jf.getContentPane().add(BorderLayout.SOUTH, jpb);

		// Disconnect:
		JButton jdiscc = new JButton("Disconnect");
		jdiscc.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
				disconnect();
			}
		});
		jpb.add(jdiscc);


		// Question:
		JButton jques = new JButton("Question");
		jques.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
			    trace("Calling IsabelClient.question()");
			    ic.question();
			}
		});
		jpb.add(jques);

		// Mute:
		JButton jmute = new JButton("Mute");
		jmute.addActionListener(new ActionListener() {
			public void actionPerformed(ActionEvent ae) {
			    trace("Calling IsabelClient.setParameter(Audio_Capture,FALSE)");
			    ic.setParameter("Audio_Capture",new Boolean(false));
			}
		});
		jpb.add(jmute);

		// Make visible:
		jf.pack();
		jf.setVisible(true);
	}

	/**
	 * Write a new trace
	 */
	void trace(String msg) {

		traces.append(msg + "\n");
	
		JScrollBar jsb = jsp.getVerticalScrollBar();
		jsb.setValue(jsb.getMaximum()); 
		
		Thread.yield();
	}

}
