/*
 * Copyright (c) 2018 Jan Van Winkel <jan.van_winkel@dxplore.eu>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <lvgl.h>
#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#include "logo.h"

#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

#define LOG_LEVEL CONFIG_LOG_DEFAULT_LEVEL
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(app);

static uint32_t count_a[10] = { 0 };
static uint32_t count;

static lv_style_t style;
static lv_style_t style2;
static lv_style_t style3;
static lv_style_t bgStyle;
static lv_style_t titleStyle;

static lv_obj_t *arc[3];
static lv_obj_t *img_logo;
static lv_obj_t *img_name;
static lv_obj_t *img_szczys;

static lv_obj_t *dis;
static uint8_t tile_idx;
static lv_obj_t *meter;

static lv_obj_t *lebel;
static lv_obj_t *title;

lv_obj_t *tv1;
lv_obj_t *tv2;
lv_obj_t *tv3;
lv_obj_t *tv4;

#ifdef CONFIG_GPIO
static struct gpio_dt_spec button_gpio = GPIO_DT_SPEC_GET_OR(
		DT_ALIAS(sw0), gpios, {0});
static struct gpio_callback button_callback;

static void button_isr_callback(const struct device *port,
				struct gpio_callback *cb,
				uint32_t pins)
{
	ARG_UNUSED(port);
	ARG_UNUSED(cb);
	ARG_UNUSED(pins);

	count = 0;
	printk("Resettings counter\n");

	if (++tile_idx > 2) {
		tile_idx = 0;
	}
	lv_obj_set_tile_id(dis, 0, tile_idx, LV_ANIM_ON);
}
#endif

static lv_obj_t * chart;
static lv_chart_series_t * ser1;

void create_chart(void) {
	/*Create a chart*/

	chart = lv_chart_create(lv_scr_act());
	lv_obj_set_size(chart, 135, 100);
	lv_obj_center(chart);
	lv_chart_set_type(chart, LV_CHART_TYPE_LINE);   /*Show lines and points too*/

	/*Add two data series*/
	ser1 = lv_chart_add_series(chart, lv_palette_main(LV_PALETTE_RED), LV_CHART_AXIS_PRIMARY_Y);

	/*Set the next points on 'ser1'*/
// 	lv_chart_set_next_value(chart, ser1, 10);
// 	lv_chart_set_next_value(chart, ser1, 10);
// 	lv_chart_set_next_value(chart, ser1, 10);
// 	lv_chart_set_next_value(chart, ser1, 10);
// 	lv_chart_set_next_value(chart, ser1, 10);
// 	lv_chart_set_next_value(chart, ser1, 10);
// 	lv_chart_set_next_value(chart, ser1, 10);
// 	lv_chart_set_next_value(chart, ser1, 30);
// 	lv_chart_set_next_value(chart, ser1, 70);
// 	lv_chart_set_next_value(chart, ser1, 90);

	/*Directly set points on 'ser2'*/
// 	ser2->y_points[0] = 90;
// 	ser2->y_points[1] = 70;
// 	ser2->y_points[2] = 65;
// 	ser2->y_points[3] = 65;
// 	ser2->y_points[4] = 65;
// 	ser2->y_points[5] = 65;
// 	ser2->y_points[6] = 65;
// 	ser2->y_points[7] = 65;
// 	ser2->y_points[8] = 65;
// 	ser2->y_points[9] = 65;
//
// 	lv_chart_refresh(chart); /*Required after direct set*/
	/*Directly set points on 'ser2'*/
	for (uint8_t i=0; i<10; i++) {
		ser1->y_points[i] = count_a[i];
	}

	lv_chart_refresh(chart); /*Required after direct set*/
}

void update_chart(void) {
    lv_chart_set_next_value(chart, ser1, ((count/10U)*10)%100);
}

static void anim_x_cb(void * var, int32_t v)
{
    lv_obj_set_x(var, v);
}

static void anim_zoom_cb(void * var, int32_t v)
{
    lv_img_set_zoom(var, v);
}

void main(void)
{
	int err;
	char count_str[11] = {0};
	const struct device *display_dev;
	lv_obj_t *hello_world_label;
	lv_obj_t *count_label;

	display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));
	if (!device_is_ready(display_dev)) {
		LOG_ERR("Device not ready, aborting test");
		return;
	}

	gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

#ifdef CONFIG_GPIO
	if (device_is_ready(button_gpio.port)) {
		err = gpio_pin_configure_dt(&button_gpio, GPIO_INPUT);
		if (err) {
			LOG_ERR("failed to configure button gpio: %d", err);
			return;
		}

		gpio_init_callback(&button_callback, button_isr_callback,
				   BIT(button_gpio.pin));

		err = gpio_add_callback(button_gpio.port, &button_callback);
		if (err) {
			LOG_ERR("failed to add button callback: %d", err);
			return;
		}

		err = gpio_pin_interrupt_configure_dt(&button_gpio,
						      GPIO_INT_EDGE_TO_ACTIVE);
		if (err) {
			LOG_ERR("failed to enable button callback: %d", err);
			return;
		}
	}
#endif
	//Initialize 3 tiles that act as pages
	lv_obj_t *scr = lv_scr_act();
	dis = lv_tileview_create(scr);
	lv_obj_align(dis, LV_ALIGN_TOP_RIGHT, 0, 0);
// 	tv1 = lv_tileview_add_tile(dis, 0, 0, LV_DIR_HOR);
// 	tv2 = lv_tileview_add_tile(dis, 0, 1, LV_DIR_HOR);
// 	tv3 = lv_tileview_add_tile(dis, 0, 2, LV_DIR_HOR);
// 	lv_obj_set_scrollbar_mode(tv1, LV_SCROLLBAR_MODE_OFF);

	/* Temporary: */
	tv2 = lv_tileview_add_tile(dis, 0, 0, LV_DIR_HOR);
	tv4 = lv_tileview_add_tile(dis, 0, 1, LV_DIR_HOR);
	tv3 = lv_tileview_add_tile(dis, 0, 2, LV_DIR_HOR);


	img_szczys = lv_img_create(tv2);

	static lv_obj_t *img_zephyr;
	img_zephyr = lv_img_create(tv4);
	lv_img_set_src(img_zephyr, &zephyr);
	lv_img_set_src(img_szczys, &szczys);

	//create style to manipulate objects characteristics implicitly
	lv_style_init(&style);
	lv_style_init(&style2);
	lv_style_init(&style3);
	lv_style_init(&bgStyle);
	lv_style_init(&titleStyle);

	lv_color_t textColor16 = lv_palette_main(LV_PALETTE_RED);	//0x014FF00	is purple
	lv_style_set_text_color(&style,textColor16);
	lv_style_set_text_font(&style,  &lv_font_montserrat_24);
	lv_style_set_text_color(&style2, lv_palette_main(LV_PALETTE_GREEN));
	lv_style_set_text_font(&style2,  &lv_font_montserrat_24);
	lv_style_set_text_color(&style3, lv_palette_main(LV_PALETTE_BLUE));
	lv_style_set_text_font(&style3,  &lv_font_montserrat_24);
	//Change background color
	textColor16 = lv_color_hex(0x000000);
	lv_obj_add_style(dis, &bgStyle, 0);
	lv_style_set_bg_color(&bgStyle, textColor16);
	//Change title text style
	textColor16 = lv_color_hex(0xFF0028);				//0xF8FCF8 is white //0xFF0028 is cyan
	lv_style_set_text_color(&titleStyle,textColor16);
	lv_style_set_text_font(&titleStyle,  &lv_font_montserrat_24);

	//Create rolling text
	lebel = lv_label_create(tv3);
	lv_obj_add_style(lebel, &style, 0);
	lv_label_set_long_mode(lebel, LV_LABEL_LONG_SCROLL_CIRCULAR);
	//To let scroll feature work properly, the text size must be larger than the assigned size.
	lv_obj_set_width(lebel, 150);
	lv_label_set_text(lebel, "This test is Red");
	lv_obj_align(lebel, LV_ALIGN_CENTER, 0, -26);
	lv_label_set_long_mode(lebel, LV_LABEL_LONG_SCROLL_CIRCULAR);

	lv_obj_t *l2 = lv_label_create(tv3);
	lv_obj_add_style(l2, &style2, 0);
	lv_label_set_text(l2, "Green text");
	lv_obj_align(l2, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t *l3 = lv_label_create(tv3);
	lv_obj_add_style(l3, &style3, 0);
	lv_label_set_text(l3, "Blue lettering");
	lv_obj_align(l3, LV_ALIGN_CENTER, 0, 26);

	lv_task_handler();
	display_blanking_off(display_dev);

	lv_style_t szczys_s;
	lv_style_init(&szczys_s);
	lv_style_set_text_color(&szczys_s, lv_color_hex(0xEC255A));
	lv_style_set_text_font(&szczys_s,  &lv_font_montserrat_32);
	lv_obj_t *label = lv_label_create(tv2);
	lv_obj_add_style(label, &szczys_s, 0);
	lv_label_set_text(label, "Mike\nSzczys");
	lv_obj_align(label, LV_ALIGN_LEFT_MID, 110, -16);

	lv_style_t title_s;
	lv_style_init(&title_s);
	lv_style_set_text_color(&title_s, lv_color_hex(0xFFFFFF));
	lv_style_set_text_font(&title_s,  &lv_font_montserrat_24);
	lv_obj_t *title_l = lv_label_create(tv2);
	lv_obj_add_style(title_l, &title_s, 0);
	lv_label_set_text(title_l, "Firmware");
	lv_obj_align(title_l, LV_ALIGN_LEFT_MID, 110, 40);

	while (1) {
		lv_task_handler();
		++count;
		k_sleep(K_MSEC(10));
	}
}
