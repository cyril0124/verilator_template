`timescale 1ns/1ns

module top(
  input  wire clk,
  input  wire rst_n,
  input  wire a,
  input  wire b,
  output wire f,
  output reg [7:0] data,

  output reg [7:0] led
);
  assign f = a ^ b;
  
  always@(posedge clk or negedge rst_n) begin
    if(rst_n == 1'b0)
      data[7:0] <= 8'h00;
    else
      data[7:0] <= data[7:0] + 1'b1;
  end


  reg [15:0] count;
  always@(posedge clk or negedge rst_n) begin
    if(rst_n == 1'b0) begin
      led[7:0] <= 8'b1000_0000;
      count <= 'd0;
    end
    else begin
      if(count == 16'hff_ff) led[7:0] <= {led[6:0],led[7]};
      count <= (count == 16'hff_ff) ? 'd0 : count + 1'b1;
    end
  end


  initial begin
      $display("\nHello World! --cyril\n");
      $finish;
  end

endmodule
