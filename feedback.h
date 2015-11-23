  typedef enum
  {
    car_fw = 0,                /**< Straight Forward */
    car_fwL = 1,        /**< Right Forward */
    car_fwR = 2,          /**< Left Forwards */
    car_stop = 3,         /**< No movement */
    car_backL = 4,                /**< Straight Back */
    car_backR = 5,              /**< Right Back */
    car_back = 6,           /**< Left Back */
    car_error = 7
  } car_state;