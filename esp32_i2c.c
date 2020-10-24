/*
 * This file is provided under a MIT license.  When using or
 * redistributing this file, you may do so under either license.
 *
 * MIT License
 *
 * Copyright (c) 2020 Pavel Nadein
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * esp32 open source driver
 *
 * Contact Information:
 * Pavel Nadein <pavelnadein@gmail.com>
 */

#include <esp32_i2c.h>

esp_err_t i2c_init(i2c_port_t i2c_num, int sda, int scl, uint32_t freq)
{
	int i2c_master_port = i2c_num;
	esp_err_t ret;
	i2c_config_t conf = {
		.mode = I2C_MODE_MASTER,
		.sda_io_num = sda,
		.sda_pullup_en = GPIO_PULLUP_ENABLE,
		.scl_io_num = scl,
		.scl_pullup_en = GPIO_PULLUP_ENABLE,
		.master.clk_speed = freq,
	};
	
	ret = i2c_param_config(i2c_master_port, &conf);
	if (ret)
		return ret;

	return i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

esp_err_t i2c_read_reg(i2c_port_t i2c_num,
	uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_rd, size_t size)
{
	i2c_cmd_handle_t cmd;
	esp_err_t ret;

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( i2c_addr << 1 ), 1);
	i2c_master_write_byte(cmd, i2c_reg, 1);
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | I2C_MASTER_READ, 1);
	if (size > 1)
		i2c_master_read(cmd, data_rd, size - 1, 0);
	i2c_master_read_byte(cmd, data_rd + size - 1, 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

esp_err_t i2c_master_write_slave_reg(i2c_port_t i2c_num,
	uint8_t i2c_addr, uint8_t i2c_reg, uint8_t* data_wr, size_t size)
{
	i2c_cmd_handle_t cmd;
	esp_err_t ret;

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, ( i2c_addr << 1 ) | I2C_MASTER_WRITE, 1);
	i2c_master_write_byte(cmd, i2c_reg, 1);
	i2c_master_write(cmd, data_wr, size, 1);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}