@matrix @testbot @mld
Feature: Basic Distance Matrix
# note that results are travel time, specified in 1/10th of seconds
# since testbot uses a default speed of 100m/10s, the result matches
# the number of meters as long as the way type is the default 'primary'

    Background:
        Given the profile "testbot"
        And the partition extra arguments "--small-component-size 1 --max-cell-sizes 4,16,64"

    Scenario: Testbot - Travel time matrix of minimal network only mld
        Given the node map
            """
            a b
            """

        And the ways
            | nodes |
            | ab    |

        When I request a travel time matrix I should get
            |   | a  | b  |
            | a | 0  | 10 |
            | b | 10 | 0  |

        When I request a travel distance matrix I should get
            |   | a   | b   |
            | a | 0   | 9.9 |
            | b | 9.9 | 0   |

    Scenario: Testbot - Travel time matrix of minimal network with excludes mld
        Given the query options
            | exclude  | toll        |

        Given the node map
            """
            a b
            c d
            """

        And the ways
            | nodes | highway  | toll | #                                        |
            | ab    | motorway |      | not drivable for exclude=motorway        |
            | cd    | primary  |      | always drivable                          |
            | ac    | highway  | yes  | not drivable for exclude=toll and exclude=motorway,toll |
            | bd    | highway  | yes  | not drivable for exclude=toll |

        When I request a travel time matrix I should get
            |   | a  | b  | c  | d  |
            | a | 0  | 15 |    |    |
            | b | 15 | 0  |    |    |
            | c |    |    | 0  | 10 |
            | d |    |    | 10 | 0  |

        When I request a travel distance matrix I should get
            |   |  a        |  b        |  c       |  d       |
            | a |  0        |  9.997721 |          |          |
            | b | 9.997721  |  0        |          |          |
            | c |           |           | 0        | 9.997724 |
            | d |           |           | 9.997724 | 0        |
