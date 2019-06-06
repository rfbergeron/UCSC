//-----------------------------------------------------------------------------
// Author: Robert Bergeron
// CruzID: rbergero
// HelloUser2.java
// Prints greeting to stdout.
//-----------------------------------------------------------------------------
class HelloUser2{
	public static void main( String[] args ){
		String userName = System.getProperty("user.name");
		
		System.out.println("Hello " + userName);
	}
}
  

