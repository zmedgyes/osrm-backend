@routing @bicycle @weight
Feature: Way type weights
	
	Background:
		Given the profile "bicycle"
	
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
		 | a    | b  | ab    | 200m +-1 | 42s +-1 |
		 | b    | a  | ab    | 200m +-1 | 42s +-1 |

	Scenario: Pick preferred route, even when it's longer
		Given the node map
		 |   | s |   |
		 | a |   | b |

		And the ways
		 | nodes | highway  |
		 | ab    | primary  |
		 | asb   | cycleway |

		When I route I should get
		 | from | to | route | distance | time    |
		 | a    | b  | asb   | 250m +-1 | 50s +-1 |
		 | b    | a  | asb   | 250m +-1 | 50s +-1 |
