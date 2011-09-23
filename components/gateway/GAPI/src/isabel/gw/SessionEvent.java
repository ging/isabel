package isabel.gw;

import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.StringTokenizer;

import isabel.lib.StringParser;

import isabel.gw.isabel_client.isabel_state.Service;

/**
 * This event holds data about a change in the isabel session such as a mode change.
 * @author Fernando Escribano
 */
public class SessionEvent {
	
	public static final String APP_SHARINNG_MODE = "application sharing";
	public static final String QUESTIONS_MODE = "questions";
	public static final String VIDEO_MODE = "video";
	public static final String IDLE_MODE = "idle";
	
    // Attributes:
    private Date   date;
    private String data;
    private int    id;
    private String mode;
    private String conference;
    private String session;
    private String participation;
    private Person [] people;
    private List<String> activeDesktops;
	
    /**
     * Constructor.
     * @param data The data that describes the event.
     * @param date The date of the event
     */
    public SessionEvent(String data, Date date) {

	this.date = date;
	this.data = data;

	this.id = 0;
	this.mode = "";
	this.conference = "";
	this.session = "";
	this.participation = "";
	this.people = new Person[0];
	this.activeDesktops = new ArrayList<String>();
	
	extract();
    }
    
    //-- Accessors

    public Date getDate() {
	return date;
    }
    
    public String getData() {
	return data;
    }

    public int getId() {
	return id;
    }

    public String getMode() {
	return mode;
    }

    public String getConference() {
	return conference;
    }

    public String getSession() {
	return session;
    }

    public String getParticipation() {
	return participation;
    }

    public Person[] getPeople() {
	return people;
    }

    public List<String> getActiveDesktops() {
    	return activeDesktops;
    }

    @Override
    public String toString() {
	return date +  ": " + data;
    }
    

    //--
	
    /**
     * Extracts the object attributes from "data". 
     */
    private void extract() {

	// Parse the data attribute.
	// This attribute contains the command used to specify an interaction mode:
	//       CM InteractionModeNotify <int_mode_name> <nsec> <siteid_list> <-opt1> <val1> <-opt2> <val2> ....
	// Important information:
	//   - int_mode_name: It is the name of the interaction mode. Used to calculate value to assign to the mode attribute.
        //   - nsec: integer assigned to the id attribute.
	//   - -scene description: Description of the scene. Description is a list of "-option value" items. See the Teleconference 
	//                         service documentation for more information.

	StringParser datasp;
	try {
	    datasp = new StringParser(data);

	    this.id = Integer.parseInt(datasp.tokenAt(3));

	    // Calculate mode:
	    Service serv = IsabelState.getIsabelState().getService();
	    String im = datasp.tokenAt(2);
	    this.mode = serv.getProperty(im,"scene.name","");
	} catch (Exception exc) {
	    return;
	}
	
	String main = datasp.tokenAt(4);
	String speakers = datasp.getOptionValue("-speakersites", main);
	
    StringTokenizer st = new StringTokenizer(speakers);
    
    while(st.hasMoreTokens()) {
    	String s = st.nextToken();
    		activeDesktops.add(s);
    }
    
    String chair = datasp.getOptionValue("-chairmansite", "");
    StringTokenizer st2 = new StringTokenizer(chair);
    
    while(st2.hasMoreTokens()) {
    	String s = st2.nextToken();
    		activeDesktops.add(s);
    }

	// Look for the value of the -scene option:
	String scene = datasp.getOptionValue("-scene", "");
	if (scene.equals("")) {
	    return;
	}

	// Parse the scene value:
	StringParser scenesp;
	try {
	    scenesp = new StringParser(scene);

	    this.conference = scenesp.getOptionValue("-conference", "");
	    this.session = scenesp.getOptionValue("-session", "");
	    this.participation = scenesp.getOptionValue("-presentation", "");

	    String site = scenesp.getOptionValue("-site", "");

	    String person = scenesp.getOptionValue("-participant", "");
	    if (person.equals("")) {
		person = scenesp.getOptionValue("-author", "");
	    }
	    if (person.equals("")) {
		person = scenesp.getOptionValue("-chair", "");
	    }
	    if (person.equals("")) {
		this.people = new Person[] {new Person("","","",site)};
	    } else {
		// Parse person values
		StringParser personsp = null;
		try {
		    personsp = new StringParser(person);
		} catch (Exception exc) {
		    return;
		}
		
		String name = personsp.tokenAt(0);
		String position = personsp.tokenAt(1);
		String organization = personsp.tokenAt(2);

		// NOTA: de momento solo envio informacion del primero. En un futuro mandare informacion sobre todos los sitios.

		this.people = new Person[] {new Person(name, position, organization, site)};
	    }
	} catch (Exception exc) {
	    return;
	}

    }	

    public class Person {
	
	private String name;
	private String position;
	private String organization;
	private String site;
	
	public Person(String name, String position, String organization, String site) {
	    this.name = name;
	    this.position = position;
	    this.organization = organization;
	    this.site = site;
	}

	public String getName() {
	    return name;
	}

	public String getPosition() {
	    return position;
	}

	public String getOrganization() {
	    return organization;
	}

	public String getSite() {
	    return site;
	}
	
    }
}
