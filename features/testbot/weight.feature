@routing @bicycle @weight
Feature: Way type weights
	
	Background:
		Given the profile "testbot"
	
	Scenario: Pick preferred route, even when longer/slower
		Given the node map
		 |   | s |   | t |   |
		 | a |   | b |   | c |

		And the ways
		 | nodes | highway | weight |
		 | ab    | primary | 1      |
		 | asb   | primary | 0.5    |
		 | bc    | primary | 2      |
		 | btc   | primary | 1      |

		When I route I should get
		 | from | to | route | distance | time    |
		 | a    | b  | asb   | 282m +-1 | 28s +-1 |
		 | b    | a  | asb   | 282m +-1 | 28s +-1 |
		 | b    | c  | btc   | 282m +-1 | 28s +-1 |
		 | c    | b  | btc   | 282m +-1 | 28s +-1 |
