class FeedBabe {
	public static void main(String[] args) {
		for(int i = 1 ; i <= 500 ; i++) {
			boolean divisible = false;
			if(i % 3 == 0) {
				System.out.print("FEED");
				divisible = true;
			}
			if(i % 4 == 0) {
				System.out.print("BABE");
				divisible = true;
			}
			if(!divisible) {
				System.out.print(i);
			}
			System.out.println("");
		}
	}
}
			
		
