@routing @bicycle @weight
Feature: Way type weights
#with the default grid sise of 100, the diagonals has a length if 141.42..

	Background:
		Given the profile "testbot"
	
	Scenario: Use weight to pick route, even when longer/slower
		Given the node map
		 |   | s |   | t |   | u |   | v |   |
		 | a |   | b |   | c |   | d |   | e |

		And the ways
		 | nodes | weight |
		 | ab    | 1.3    |
		 | asb   | 1      |
		 | bc    | 1.5    |
		 | btc   | 1      |
		 | cd    | 0.015  |
		 | cud   | 0.010  |
		 | de    | 150000 |
		 | dve   | 100000 |

		When I route I should get
		 | from | to | route | distance |
		 | a    | b  | ab    | 200m +-1 |
		 | b    | a  | ab    | 200m +-1 |
		 | b    | c  | btc   | 282m +-1 |
		 | c    | b  | btc   | 282m +-1 |
		 | c    | d  | cud   | 282m +-1 |
		 | d    | c  | cud   | 282m +-1 |
		 | d    | e  | dve   | 282m +-1 |
		 | e    | d  | dve   | 282m +-1 |

 	Scenario: Weight should default to 1
 		Given the node map
 		 |   | s |   | t |   |
 		 | a |   | b |   | c |

 		And the ways
 		 | nodes | weight |
 		 | ab    | 1.40   |
 		 | asb   |        |
 		 | bc    | 1.42   |
 		 | btc   |        |

 		When I route I should get
 		 | from | to | route |
 		 | a    | b  | ab    |
 		 | b    | a  | ab    |
 		 | b    | c  | btc   |
 		 | c    | b  | btc   |

  	Scenario: Use both weight and speed (multiplied) when picking route
  		Given the node map
  		 |   | s |   | t |   |
  		 | a |   | b |   | c |

  		And the ways
  		 | nodes | weight | highway   |
  		 | ab    | 2.80   | primary   |
  		 | asb   | 1      | secondary |
  		 | bc    | 2.84   | primary   |
  		 | btc   | 1      | secondary |

  		When I route I should get
  		 | from | to | route |
  		 | a    | b  | ab    |
  		 | b    | a  | ab    |
  		 | b    | c  | btc   |
  		 | c    | b  | btc   |

 	Scenario: Weight should not influence speed (or travel time)
 		Given the node map
 		 | a | b | c |

 		And the ways
 		 | nodes | weight |
 		 | ab    | 1      |
 		 | bc    | 2      |

 		When I route I should get
 		 | from | to | route | distance | time |
 		 | a    | b  | ab    | 100m +-1 | 10s  |
 		 | b    | a  | ab    | 100m +-1 | 10s  |
 		 | b    | c  | bc    | 100m +-1 | 10s  |
 		 | c    | b  | bc    | 100m +-1 | 10s  |
 		 | a    | c  | ab,bc | 200m +-1 | 20s  |
 		 | c    | a  | bc,ab | 200m +-1 | 20s  |
