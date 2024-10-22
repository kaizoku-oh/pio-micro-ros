/*-----------------------------------------------------------------------------------------------*/
/* Includes                                                                                      */
/*-----------------------------------------------------------------------------------------------*/
#include <Arduino.h>

#include "micro_ros_platformio.h"
#include "rcl/rcl.h"
#include "rclc/rclc.h"
#include "rclc/executor.h"
#include "std_msgs/msg/u_int8.h"

/*-----------------------------------------------------------------------------------------------*/
/* Defines                                                                                       */
/*-----------------------------------------------------------------------------------------------*/
#ifndef LED_BUILTIN
#define LED_BUILTIN (2)
#endif // LED_BUILTIN

#ifndef USER_BTN
#define USER_BTN (0)
#endif // USER_BTN

/*-----------------------------------------------------------------------------------------------*/
/* Macros                                                                                        */
/*-----------------------------------------------------------------------------------------------*/
#define RC_CHECK(fn) { rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) { errorLoop(); }}
#define RC_SOFT_CHECK(fn) { rcl_ret_t temp_rc = fn; if ((temp_rc != RCL_RET_OK)) {} }

/*-----------------------------------------------------------------------------------------------*/
/* Private functions prototypes                                                                  */
/*-----------------------------------------------------------------------------------------------*/
static void onButtonPressed();
static void onLedMessageReceived(const void *msg);
static void errorLoop();

/*-----------------------------------------------------------------------------------------------*/
/* Private global variables                                                                      */
/*-----------------------------------------------------------------------------------------------*/
static rcl_node_t node = {0};
static rclc_support_t support = {0};
static rcl_allocator_t allocator = {0};
static rclc_executor_t executor = {0};
static rcl_publisher_t publisher = {0};
static std_msgs__msg__UInt8 buttonPressCounterMessage = {0};
static rcl_subscription_t ledSubscription = {0};
static std_msgs__msg__UInt8 ledSubscriptionMessage = {0};

static volatile bool buttonIsPressed = false;

/*-----------------------------------------------------------------------------------------------*/
/* Private constants                                                                             */
/*-----------------------------------------------------------------------------------------------*/
static const uint32_t SERIAL_BAUDRATE = 115200;

// The total number of subscriptions, timers, services
static const uint8_t NUMBER_OF_EXECUTOR_HANDLES = RMW_UXRCE_MAX_SUBSCRIPTIONS + RMW_UXRCE_MAX_SERVICES;

/*-----------------------------------------------------------------------------------------------*/
/* Public functions                                                                              */
/*-----------------------------------------------------------------------------------------------*/
/**
  * @brief Called from inside the Arduino framework at the start of main()
  * @param None
  * @retval None
  */
void setup() {
  // Setup LED
  pinMode(LED_BUILTIN, OUTPUT);

  // Setup button pin and interrupt
  pinMode(USER_BTN, INPUT);
  attachInterrupt(digitalPinToInterrupt(USER_BTN), onButtonPressed, RISING);

  // Setup serial transport
  Serial.begin(SERIAL_BAUDRATE);

  // Set micro-ros custom transport
  set_microros_serial_transports(Serial);
  delay(2000);

  // Create a properly initialized allocator with default values
  allocator = rcl_get_default_allocator();

  // Create init_options
  RC_CHECK(rclc_support_init(&support, 0, NULL, &allocator));

  // Create node
  RC_CHECK(rclc_node_init_default(&node, "pio_micro_ros", "", &support));

  // Create button publisher
  RC_CHECK(rclc_publisher_init_default(
    &publisher,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
    "button"));

  // Create LED subscription
  RC_CHECK(rclc_subscription_init_default(
    &ledSubscription,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, UInt8),
    "led"));

  // Create executor
  RC_CHECK(rclc_executor_init(&executor, &support.context, NUMBER_OF_EXECUTOR_HANDLES, &allocator));

  // Add subscriptions to executor
  RC_CHECK(rclc_executor_add_subscription(
    &executor,
    &ledSubscription,
    &ledSubscriptionMessage,
    &onLedMessageReceived,
    ON_NEW_DATA));
}

/**
  * @brief Called repeatedly from inside the Arduino framework main()
  * @param None
  * @retval None
  */
void loop() {
  // Continuously checks for new data from the DDS-queue
  RC_SOFT_CHECK(rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100)));
  // Check if button is pressed
  if (buttonIsPressed) {
    buttonIsPressed = false;
    buttonPressCounterMessage.data++;
    // Publish button counter message
    RC_SOFT_CHECK(rcl_publish(&publisher, &buttonPressCounterMessage, NULL));
  }
}

/*-----------------------------------------------------------------------------------------------*/
/* Private functions                                                                             */
/*-----------------------------------------------------------------------------------------------*/
/**
  * @brief Button pressed ISR callback
  * @param None
  * @retval None
  */
static void onButtonPressed() {
  buttonIsPressed = true;
}

/**
  * @brief LED subscription callback
  * @param msg Received message
  * @retval None
  */
static void onLedMessageReceived(const void *msg) {
  const std_msgs__msg__UInt8 *message = (const std_msgs__msg__UInt8 *)msg;

  digitalWrite(LED_BUILTIN, (message->data == 0) ? LOW : HIGH);
}

/**
  * @brief Error handler function
  * @param None
  * @retval None
  */
static void errorLoop() {
  Serial.println("Error!");
  while (true) {
    delay(100);
  }
}
