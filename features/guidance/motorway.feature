@routing  @guidance
Feature: Motorway Guidance

    Background:
        Given the profile "car"
        Given a grid size of 10 meters

    Scenario: Ramp Exit Right
        Given the node map
            """
            a-b-c-d-e
               `--f-g
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | bfg   | motorway_link | yes    |

       When I route I should get
            | waypoints | route         | turns                                |
            | a,e       | abcde,abcde   | depart,arrive                        |
            | a,g       | abcde,bfg,bfg | depart,off ramp slight right,arrive |

    Scenario: Ramp Exit Right Curved Right
        Given the node map
            """
            a-b-c
               `f`d
                 `g`e
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | bfg   | motorway_link | yes    |

       When I route I should get
            | waypoints | route         | turns                         |
            | a,e       | abcde,abcde   | depart,arrive                 |
            | a,g       | abcde,bfg,bfg | depart,off ramp right,arrive |

    Scenario: Ramp Exit Right Curved Left
        Given the node map
            """
                   ,e
                 ,d,g
            a-b-c-f
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | cfg   | motorway_link | yes    |

       When I route I should get
            | waypoints | route         | turns                                |
            | a,e       | abcde,abcde   | depart,arrive                        |
            | a,g       | abcde,cfg,cfg | depart,off ramp slight right,arrive |


    Scenario: Ramp Exit Left
        Given the node map
            """
               /--f-g
            a-b-c-d-e
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | bfg   | motorway_link | yes    |

       When I route I should get
            | waypoints | route         | turns                               |
            | a,e       | abcde,abcde   | depart,arrive                       |
            | a,g       | abcde,bfg,bfg | depart,off ramp slight left,arrive |

    Scenario: Ramp Exit Left Curved Left
        Given the node map
            """
                 ,g,e
               ,f,d
            a-b-c 
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | bfg   | motorway_link | yes    |

       When I route I should get
            | waypoints | route         | turns                        |
            | a,e       | abcde,abcde   | depart,arrive                |
            | a,g       | abcde,bfg,bfg | depart,off ramp left,arrive |

    Scenario: Ramp Exit Left Curved Right
        Given the node map
            """
            a-b-c-f
                 `d`g
                   `e
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | cfg   | motorway_link | yes    |

       When I route I should get
            | waypoints | route         | turns                               |
            | a,e       | abcde,abcde   | depart,arrive                       |
            | a,g       | abcde,cfg,cfg | depart,off ramp slight left,arrive |

    Scenario: On Ramp Right
        Given the node map
            """
            a-b-c-d-e
            f-g---'
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | fgd   | motorway_link | yes    |

       When I route I should get
            | waypoints | route           | turns                           |
            | a,e       | abcde,abcde     | depart,arrive                   |
            | f,e       | fgd,abcde,abcde | depart,merge slight left,arrive |

    Scenario: On Ramp Left
        Given the node map
            """
            f-g---,
            a-b-c-d-e
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | fgd   | motorway_link | yes    |

       When I route I should get
            | waypoints | route           | turns                            |
            | a,e       | abcde,abcde     | depart,arrive                    |
            | f,e       | fgd,abcde,abcde | depart,merge slight right,arrive |

    Scenario: Highway Fork
        Given the node map
            """
                 /--d-e
            a-b-c
                 \--f-g
            """

        And the ways
            | nodes  | highway  |
            | abcde  | motorway |
            | cfg    | motorway |

       When I route I should get
            | waypoints | route             | turns                           |
            | a,e       | abcde,abcde,abcde | depart,fork slight left,arrive  |
            | a,g       | abcde,cfg,cfg     | depart,fork slight right,arrive |

     Scenario: Fork After Ramp
       Given the node map
            """
                 /--d-e
            a-b-c
                 \--f-g
            """

        And the ways
            | nodes | highway       | oneway |
            | abc   | motorway_link | yes    |
            | cde   | motorway      |        |
            | cfg   | motorway      |        |

       When I route I should get
            | waypoints | route       | turns                           |
            | a,e       | abc,cde,cde | depart,fork slight left,arrive  |
            | a,g       | abc,cfg,cfg | depart,fork slight right,arrive |

     Scenario: On And Off Ramp Right
       Given the node map
            """
            a-b---c---d-e
            f-g--/ \--h i
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | fgc   | motorway_link | yes    |
            | chi   | motorway_link | yes    |

       When I route I should get
            | waypoints | route           | turns                              |
            | a,e       | abcde,abcde     | depart,arrive                      |
            | f,e       | fgc,abcde,abcde | depart,merge slight left,arrive    |
            | a,i       | abcde,chi,chi   | depart,off ramp slight right,arrive |
            | f,i       | fgc,chi,chi     | depart,off ramp right,arrive       |

    Scenario: On And Off Ramp Left
       Given the node map
            """
            f-g--\/---h-i
            a-b---c---d-e
            """

        And the ways
            | nodes | highway       | oneway |
            | abcde | motorway      |        |
            | fgc   | motorway_link | yes    |
            | chi   | motorway_link | yes    |

       When I route I should get
            | waypoints | route           | turns                             |
            | a,e       | abcde,abcde     | depart,arrive                     |
            | f,e       | fgc,abcde,abcde | depart,merge slight right,arrive  |
            | a,i       | abcde,chi,chi   | depart,off ramp slight left,arrive |
            | f,i       | fgc,chi,chi     | depart,off ramp left,arrive       |

    Scenario: Merging Motorways
        Given the node map
            """
            e\
            a-b-c
            d/
            """

        And the ways
            | nodes | highway  |
            | abc   | motorway |
            | db    | motorway |
            | eb    | motorway |

        When I route I should get
            | waypoints | route      | turns                            |
            | d,c       | db,abc,abc | depart,merge slight left,arrive  |
            | e,c       | eb,abc,abc | depart,merge slight right,arrive |

    Scenario: Handle 90 degree off ramps correctly
        Given the node map
            """
            a\
            x-b---c-y
                  d
            """

        And the ways
            | nodes | name | highway       | oneway |
            | ab    | On   | motorway_link | yes    |
            | xb    | Hwy  | motorway      |        |
            | bc    | Hwy  | motorway      |        |
            | cd    | Off  | motorway_link | yes    |
            | cy    | Hwy  | motorway      |        |

       When I route I should get
            | waypoints | route               | turns                                           |
            | a,d       | On,Hwy,Off,Off      | depart,merge slight right,off ramp right,arrive |

    #http://0.0.0.0:9966/?z=18&center=38.893323%2C-77.055117&loc=38.893551%2C-77.054833&loc=38.893112%2C-77.055536&hl=en&alt=0
    Scenario: Merging with same name
        Given the node map
            """
            a - - -
                    > c - d
                 b
            """

        And the ways
            | nodes | name | ref         | highway  | oneway |
            | ac    |      | US 50       | motorway | yes    |
            | bc    |      | I 66        | motorway | yes    |
            | cd    |      | US 50; I 66 | motorway | yes    |

        When I route I should get
            | waypoints | route | turns         |
            | a,d       | ,     | depart,arrive |
            | b,d       | ,     | depart,arrive |

     @sub-exit
     Scenario: Subexit
        Given the node map
            """
            a - - b - - - - - - - - - - - - - - - - - - - c
                    ` d - - - - e - - - - f
                                  ' g - - h
            """

        And the ways
            | nodes | name          | ref  | highway       | lanes | oneway |
            | abc   | motorway      | US 1 | motorway      | 6     | yes    |
            | bdef  | ramp          |      | motorway_link | 2     | yes    |
            | egh   | sub-ramp      |      | motorway_link | 1     | yes    |

        When I route I should get
            | waypoints | route                           | turns                                                 |
            | a,c       | motorway,motorway               | depart,arrive                                         |
            | a,f       | motorway,ramp,ramp              | depart,off ramp slight right,arrive                   |
            | a,h       | motorway,ramp,sub-ramp,sub-ramp | depart,off ramp slight right,turn slight right,arrive |

     Scenario: Subentry
        Given the node map
            """
                    k
                    |
                   |-|
            a - - - - - - - - - - - - - - - - - - b - - c
                   |-|
                    d - - - - e - - - - f
                   |-|
            g - - - - - - - - - - - - - h - - - - - - - - - - i
                   |-|
                    |
                    j
            """

        And the ways
            | nodes | name           | ref  | highway       | lanes | oneway |
            | abc   | motorway north | US 1 | motorway      | 6     | yes    |
            | defb  | on-ramp        |      | motorway_link | 2     | yes    |
            | eh    | sub-ramp       |      | motorway_link | 1     | yes    |
            | ghi   | motorway south | US 2 | motorway      | 6     | yes    |
            | jdk   | road           |      | primary       |       | no     |

        When I route I should get
            | waypoints | route                                               | turns                                                            |
            | a,c       | motorway north,motorway north                       | depart,arrive                                                    |
            | j,c       | road,on-ramp,motorway north,motorway north          | depart,on ramp right,merge slight left,arrive                    |
            | j,i       | road,on-ramp,sub-ramp,motorway south,motorway south | depart,on ramp right,turn slight right,merge slight right,arrive |
            | g,i       | motorway south,motorway south                       | depart,arrive                                                    |
