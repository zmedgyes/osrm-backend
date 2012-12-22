@routing @bicycle @weight
Feature: Way type weights
	
	Background:
		Given the profile "weight"
	
	Scenario: Pick the geometrically shortest route, way types being equal
		Given the node map
		 |   | s |   |
		 | a |   | b |

		And the ways
		 | nodes | highway |
		 | ab    | primary |
		 | asb   | primary |

		When I route I should get
		 | from | to | route | distance | time    |
		 | a    | b  | ab    | 200m +-2 | 20s +-1 |
		 | b    | a  | ab    | 200m +-2 | 20s +-1 |

	Scenario: Pick preferred route, even when it's longer
		Given the node map
		 |   | s |   |
		 | a |   | b |

		And the ways
		 | nodes | highway   |
		 | ab    | primary   |
		 | asb   | secondary |

		When I route I should get
		 | from | to | route | distance | time    |
		 | a    | b  | asb   | 282m +-2 | 28s +-1 |
		 | b    | a  | asb   | 282m +-2 | 28s +-1 |
